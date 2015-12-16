class WifiController
{
private:
	ESP* _esp = nullptr;
	MQTT* _mqtt = nullptr;
	bool _wifiConnected = false;

	volatile bool _msgReady = false;
	volatile bool _loop = false;
	volatile uint32_t _color;
	volatile int32_t _position;

	KeyFrame _frames[MAX_KEYFRAMES];
	volatile size_t _animationSize = 0;

public:
	void begin(bool debug);
	void process();
	volatile bool isMsgReady();
	volatile bool loop();
	size_t copyFrames(KeyFrame frames[]);
	void getMsg(uint32_t& color, int32_t& position);

	void wifiCb(void* response);
	void mqttConnected(void* response);
	void mqttDisconnected(void* response);
	void mqttData(void* response);
	void mqttPublished(void* response);

	void wifiInit();

private:

};


void WifiController::begin(bool debug)
{
	if (debug)
	{
		_esp = new ESP(&Serial1, &Serial, ESP_RESET); // &ESP(&Serial1, &Serial, ESP_RESET);
	}
	else
	{
		_esp = new ESP(&Serial1, ESP_RESET);
	}
	_mqtt = new MQTT(_esp);

	wifiInit();
}

void WifiController::process()
{
	_esp->process();
}

volatile bool WifiController::isMsgReady()
{
	return _msgReady;
}

volatile bool WifiController::loop()
{
	return _loop;
}

size_t WifiController::copyFrames(KeyFrame frames[])
{
	_msgReady = false;
	memcpy(frames, _frames, sizeof(KeyFrame) * _animationSize);
	return _animationSize;
}

void WifiController::getMsg(uint32_t& color, int32_t& position)
{
	_msgReady = false; //reset
	color = _color;
	position = _position;
}

void WifiController::wifiCb(void* response)
{
	uint32_t status;
	RESPONSE res(response);

	if (res.getArgc() == 1) {
		res.popArgs((uint8_t*)&status, 4);
		if (status == STATION_GOT_IP) {
			DEBUG_PRINTLN("WIFI CONNECTED");
			_mqtt->connect(MQTT_BROKER, MQTT_PORT, MQTT_SECURE);
			_wifiConnected = true;
		}
		else {
			_wifiConnected = false;
			_mqtt->disconnect();
		}
	}
}

void WifiController::mqttConnected(void* response)
{
	DEBUG_PRINTLN("Mqtt: Connected");
	_mqtt->subscribe(TOPIC_COMMAND);
	_mqtt->publish(TOPIC_STATUS, "ON");
}

void WifiController::mqttDisconnected(void* response)
{
	DEBUG_PRINTLN("Mqtt: disconected from service");
	// TODO: show state and try reconect
}

void WifiController::mqttData(void* response)
{
	RESPONSE res(response);
	String topic = res.popString();
	String jsonData = res.popString();

	if (topic == TOPIC_COMMAND)
	{
		DEBUG_PRINTLN(jsonData);
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(jsonData);
		if (root.success())
		{
			if (root.containsKey("c"))
			{
				// ? loop
				/*if (strcmp(root["l"].asString(),"true"))
				_loop = false;
				else
				_loop = true;*/
				//_loop = root["l"];
				String loop = root["l"].as<String>();
				if (loop.equals("false"))
					_loop = false;
				else
					_loop = true;
				// parse animation sequence
				JsonArray& cmdArray = root["c"].asArray();
				for (size_t i = 0; i < cmdArray.size(); i++)
				{
					//get animation type
					_frames[i].state = (ControllState)cmdArray[i]["a"].as<uint32_t>();

					//get color
					String htmlColor = cmdArray[i]["c"].as<String>();
					// Get rid of '#' and convert it to integer
					uint32_t number = (uint32_t)strtol(&htmlColor[1], NULL, 16);
					//htmlColor.remove(0, 1); // Remove # from html rep
					//htmlColor.replace('#', 'x');
					//htmlColor = '0' + htmlColor;

					DEBUG_PRINTLN(number);
					_frames[i].color.setColorCode(number);
					DEBUG_PRINTLN(_frames[i].color);

					//get motor settings
					_frames[i].position = cmdArray[i]["m"]["p"].as<uint32_t>();
					_frames[i].speed = cmdArray[i]["m"]["s"].as<uint32_t>();
					_frames[i].acceleration = cmdArray[i]["m"]["a"].as<uint32_t>();

					// get timings
					_frames[i].timeOut = cmdArray[i]["t"]["l"].as<uint32_t>();
					_frames[i].rate = cmdArray[i]["t"]["r"].as<uint32_t>();
				}
				//DEBUG_PRINTLN(cmdArray[0]["c"][0].as<>());
				_animationSize = cmdArray.size();
				_msgReady = true;
			}

			DEBUG_PRINTLN("Set new color & position");
		}
	}
}

void WifiController::mqttPublished(void* response)
{
	DEBUG_PRINTLN("Published something");
}

// Init wifi
void WifiController::wifiInit()
{
	DEBUG_PRINTLN("Teensy: resetting esp");
	_esp->disable();
	delay(500);
	_esp->enable();
	delay(500);
	_esp->reset();
	delay(500);
	while (!_esp->ready());

	// Setup Mqtt client
	DEBUG_PRINTLN("Teensy: setup mqtt client");
	if (!_mqtt->begin("abb_pslamp1", "", "", 60, 1)) {
		DEBUG_PRINTLN("Teensy: fail to setup mqtt");
		while (1); //reset
	}

	// Set last will and testament
	DEBUG_PRINTLN("ARDUINO: setup mqtt lwt");
	_mqtt->lwt("/lwt", "offline", 0, 0);

	// Mqtt callbacks
	_mqtt->connectedCb.attach(this, &WifiController::mqttConnected);
	_mqtt->disconnectedCb.attach(this, &WifiController::mqttDisconnected);
	_mqtt->publishedCb.attach(this, &WifiController::mqttPublished);
	_mqtt->dataCb.attach(this, &WifiController::mqttData);

	// Connect to wifi
	DEBUG_PRINTLN("Teensy: setup wifi");
	_esp->wifiCb.attach(this, &WifiController::wifiCb);
	_esp->wifiConnect(WIFI_SSID, WIFI_PASSWORD);
	DEBUG_PRINTLN("Teensy: system started");
}


#pragma once

#define DEBUG
//#define END_STOP_ACTIVE
//#define ABB 

#ifdef DEBUG
#define DEBUG_PRINT(x)     Serial.print (x)
#define DEBUG_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x) 
#endif

#define MQTT_SECURE false
#define MQTT_BROKER "test.mosquitto.org"
#define MQTT_PORT   1883   // mqtt broker port

#ifdef ABB
#define WIFI_PASSWORD	"bollkalle"
#define WIFI_SSID		"vejron"
#define MQTT_CLIENT_ID1	"abb_pslamp1"
#define MQTT_CLIENT_ID2	"abb_pslamp2"
#define TOPIC_COMMAND	"/abbps/ver100/2/c"
#define TOPIC_READY		"/abbps/ver100/2/r"
#define TOPIC_STATUS	"/abbps/ver100/2/s"
#define NUM_LEDS_PER_STRIP  50
#define NUM_STRIPS          1
#define MAX_BRIGHTNESS      200
#else
#define WIFI_PASSWORD	"motherbrain"
#define WIFI_SSID		"ASUS"
#define MQTT_CLIENT_ID1	"pslamp1"
#define MQTT_CLIENT_ID2	"pslamp2"
#define TOPIC_COMMAND	"/vejronps/ver101/1/c"
#define TOPIC_READY		"/vejronps/ver101/1/r"
#define TOPIC_STATUS	"/vejronps/ver101/1/s"
#define NUM_LEDS_PER_STRIP  40
#define NUM_STRIPS          1
#define MAX_BRIGHTNESS      255
#endif

#define MAX_KEYFRAMES		50
#define STEPPER_MAX_SPEED			350
#define STEPPER_MAX_ACCELERATION	250

#define STEPPER_END_PIN 9	// end stop for callibration
#define ESP_RESET		3	// ESP8266 enable pin
#define MOTORPIN1   22		// IN1 on the ULN2803A 
#define MOTORPIN2   19		// IN2 on the ULN2803A 
#define MOTORPIN3   18		// IN3 on the ULN2803A 
#define MOTORPIN4   17		// IN4 on the ULN2803A 

//#define HALFSTEP    8
#ifndef HALFSTEP
#define STEPS_PER_OUTPUT_REVOLUTION 2041	//shuld be 64 * 32 but gear ratio is a bit off (cheap shit)
#else
#define	STEPS_PER_OUTPUT_REVOLUTION (2041 * 2)
#endif

#define STEPPER_SHAFT_LENGTH 92.0

#ifdef ABB
#define __D		15.0f
#else
#define __D		10.0f
#endif // ABB

//const float ___d = 15;
const float ___C = PI * __D;
const int STEPPER_RANGE = (STEPPER_SHAFT_LENGTH / ___C) * STEPS_PER_OUTPUT_REVOLUTION;  // 7969

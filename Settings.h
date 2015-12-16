#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(x)     Serial.print (x)
#define DEBUG_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x) 
#endif

#define WIFI_PASSWORD "motherbrain"
#define WIFI_SSID     "ASUS"

#define MQTT_SECURE false
#define MQTT_BROKER "test.mosquitto.org"
#define MQTT_PORT   1883   // mqtt broker port
#define ESP_RESET   3      // pin 3 enable esp

#define TOPIC_COMMAND "/abbps/ver101/2/c"
#define TOPIC_READY   "/abbps/ver101/2/r"
#define TOPIC_STATUS  "/abbps/ver101/2/s"

#define MAX_KEYFRAMES	50

#define NUM_LEDS_PER_STRIP  50
#define NUM_STRIPS          1
#define MAX_BRIGHTNESS      200

#define HALFSTEP    8
#define MOTORPIN1   22     // IN1 on the ULN2803A 
#define MOTORPIN2   19     // IN2 on the ULN2803A 
#define MOTORPIN3   18     // IN3 on the ULN2803A 
#define MOTORPIN4   17     // IN4 on the ULN2803A 

#define STEPPER_END_PIN 9

#ifndef HALFSTEP
#define STEPS_PER_OUTPUT_REVOLUTION 2041	//shuld be 64 * 32 but gear ratio is a bit off (cheap shit)
#else
#define	STEPS_PER_OUTPUT_REVOLUTION (2041 * 2)
#endif

#define STEPPER_SHAFT_LENGTH 92.0

const float ___d = 15;
const float ___C = PI * ___d;
const int STEPPER_MIN = (STEPPER_SHAFT_LENGTH / ___C) * STEPS_PER_OUTPUT_REVOLUTION;  // 7969

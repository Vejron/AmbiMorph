//#define F_TEST
#define USE_OCTOWS2811    // Important! make fastLED use octoWS2811
#include <AccelStepper.h>
#include <OctoWS2811.h>
#include <FastLED.h>
#include <ArduinoJson.h>
#include <espduino.h>
#include <mqtt.h>

#include "Settings.h"
#include "Common.h"

#include "Bulb.h"
#include "Winch.h"
#include "Wifi.h"
#include "Animator.h"


// LED strip stuff
Bulb bulb;

// Stepper stuff
Winch winch;

// Wifi
WifiController wifi;

// Controller
Animator animator;

// timer
elapsedMillis sinceLastIsr;

// Create an IntervalTimer object 
IntervalTimer stepTimer;

void priorityTask() {
	winch.run();
	animator.run();

	if (sinceLastIsr > 5000)
	{
		sinceLastIsr = 0;
		DEBUG_PRINTLN(millis());
	}
}

void setup() {
	// setup serial ports
	Serial1.begin(19200);	// for esp8266
	Serial.begin(115200);	// debug port

							// reset and setup everything
	winch.begin();
	bulb.begin();
	wifi.begin(true);
	animator.begin(&bulb, &winch, &wifi);

	// start interrupt routine
	sinceLastIsr = 0;
	stepTimer.begin(priorityTask, 1000);
}

void loop() {
	// Handle esp comunication
	wifi.process();
}
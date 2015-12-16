#include <AccelStepper.h>
#include <ArduinoJson.h>
#include <espduino.h>

#include "Settings.h"
#include "Winch.h"

// stepper
Winch winch;

// timer
elapsedMillis sinceLastIsr;

// create an IntervalTimer object 
IntervalTimer stepTimer;

void priorityTask() {
	winch.run();
	//animator.run();

	if (sinceLastIsr > 5000)
	{
		sinceLastIsr = 0;
		DEBUG_PRINTLN(millis());
	}
}

void setup()
{
	// setup serial ports
	Serial1.begin(19200);	// for esp8266
	Serial.begin(115200);	// debug port

							// reset and setup everything
	winch.begin();
	//bulb.begin();
	//wifi.begin(true);
	//animator.begin(&bulb, &winch, &wifi);

	// start interrupt routine
	sinceLastIsr = 0;
	stepTimer.begin(priorityTask, 1000);
}

void loop()
{

  /* add main program code here */

}

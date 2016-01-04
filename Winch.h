#pragma once

class Winch
{
private:
	volatile bool m_calibrated = false;
	volatile bool m_enabled = true;
	volatile int m_target;
	volatile int m_current;
	AccelStepper m_stepper = AccelStepper(AccelStepper::HALF4WIRE, MOTORPIN4, MOTORPIN2, MOTORPIN3, MOTORPIN1, true);

	inline float clamp(int x, int a, int b)
	{
		return x < a ? a : (x > b ? b : x);
	}

	void setTargetRaw(volatile int target)
	{
		if (target != m_target)
		{
			if (target >= -STEPPER_MIN && target <= 0) // Has to be within limits
			{
				m_target = target;
			}
			else
			{
				m_target = clamp(target, -STEPPER_MIN, 0);
				DEBUG_PRINTLN("Warning: not in range capping");
			}
			enableOutputs();
			m_stepper.moveTo(m_target);
		}
	}

	inline void disableOutputs()
	{
		m_stepper.disableOutputs();
		m_enabled = false;
	}

	inline void enableOutputs()
	{
		m_enabled = true;
	}

public:
	void begin()
	{
#ifdef END_STOP_ACTIVE
		pinMode(STEPPER_END_PIN, INPUT);
		pinMode(STEPPER_END_PIN, INPUT_PULLUP);
		CalibrateSynch();
#else
		calibrateOpen();
	}

	void calibrateOpen()
	{
		m_calibrated = false;
		m_stepper.setAcceleration(500.0);
		m_stepper.setMaxSpeed(1000);
		m_stepper.setSpeed(1000);						// full trottle, making the stepper intentionally a bit less powerfull :)
		DEBUG_PRINTLN("calibration started");
		m_stepper.runToNewPosition(STEPPER_MIN);		// hoist all the way. hopefully nothing will break :)
		m_stepper.setSpeed(300);
		DEBUG_PRINTLN("releasing tension");
		m_stepper.runToNewPosition(STEPPER_MIN - 150);	// back down a bit to release tension

														
		m_stepper.setCurrentPosition(0);				// at start position. calibrate here
		m_stepper.setMaxSpeed(1000.0);
		m_stepper.setAcceleration(500.0);
		disableOutputs();
		m_calibrated = true;
	}
#endif

	void calibrateSynch()
	{
		m_calibrated = false;
		m_stepper.setAcceleration(500.0);
		m_stepper.setMaxSpeed(1000);
		m_stepper.setSpeed(1000);           // Not to fast :)
		DEBUG_PRINTLN("calibration started");
		while (digitalRead(STEPPER_END_PIN)) // at endstop yet?
		{
			m_stepper.runSpeed();
		}
		delay(10); // debounce
		DEBUG_PRINTLN("endstop reached");

		//_stepper.setSpeed(-200);             // the other way
		//while(!digitalRead(STEPPER_END_PIN)) // back down a little
		//{
		//  _stepper.runSpeed();
		//}
		m_stepper.setCurrentPosition(0);
		m_stepper.setSpeed(-300);
		m_stepper.runToNewPosition(-150);
		DEBUG_PRINTLN("calibrated");
		DEBUG_PRINT("STEPPER_MIN: "); DEBUG_PRINTLN(STEPPER_MIN);

		// At start position. Calibrate
		m_stepper.setCurrentPosition(0);
		// set default values
		m_stepper.setMaxSpeed(1000.0);
		m_stepper.setAcceleration(500.0);
		disableOutputs();
		m_calibrated = true;
	}

	void setTarget(int target, float speed, float acceleration)
	{
		if (m_calibrated)
		{
			m_stepper.setAcceleration(acceleration);
			m_stepper.setMaxSpeed(speed);
			setTargetRaw(-STEPPER_MIN * (target / 100.0));
			//setTargetRaw(-target);
		}
		else
		{
			DEBUG_PRINTLN("Warning: not calibrated, no movement possible");
		}
	}

	bool isAtTarget()
	{
		if (m_stepper.distanceToGo() == 0) return true;
		else return false;
	}

	void run()
	{
#ifdef END_STOP_ACTIVE
		if (!digitalRead(STEPPER_END_PIN))
		{
			m_calibrated = false;
			//Todo: out of calibration
			DEBUG_PRINTLN("Error: out of calibration, restart");
		}
#endif
		if (m_calibrated)
		{
			if (!m_stepper.run())
			{
				//at target, disable motor to make it less hot
				disableOutputs();
				//DEBUG_PRINTLN("Info: at target position");

			}
		}
		else
		{
			disableOutputs();
		}
	}
};
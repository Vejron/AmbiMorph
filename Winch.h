#pragma once

class Winch
{
private:
	volatile bool m_calibrationInProgress = false;
	bool m_stopping = false;
	volatile bool m_calibrated = false;
	volatile bool m_enabled = true;
	volatile int m_target;
	volatile int m_current;
#ifdef ABB
	AccelStepper m_stepper = AccelStepper(AccelStepper::HALF4WIRE, MOTORPIN4, MOTORPIN2, MOTORPIN3, MOTORPIN1, true);
#else
	AccelStepper m_stepper = AccelStepper(AccelStepper::FULL4WIRE, MOTORPIN4, MOTORPIN2, MOTORPIN3, MOTORPIN1, true);
#endif // ABB

	inline float clamp(int x, int a, int b)
	{
		return x < a ? a : (x > b ? b : x);
	}

	void setTargetRaw(volatile int target)
	{
		if (target != m_target)
		{
			if (target >= -STEPPER_RANGE && target <= 0) // Has to be within limits
			{
				m_target = target;
			}
			else
			{
				m_target = clamp(target, -STEPPER_RANGE, 0);
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

	bool calibrate()
	{
		if (!m_calibrationInProgress)
		{
			m_calibrationInProgress = true;
			m_stepper.setCurrentPosition(0);
			m_stepper.setAcceleration(STEPPER_MAX_ACCELERATION);
			m_stepper.setMaxSpeed(STEPPER_MAX_SPEED);
			m_stepper.moveTo(STEPPER_RANGE);
		}
		else if (isAtTarget())
		{
			m_calibrationInProgress = false;
			m_stepper.setCurrentPosition(0);
			return true;
		}

		return false;
	}

	void calibrateOpen()
	{
		m_calibrated = false;
		m_stepper.setAcceleration(200.0);
		m_stepper.setMaxSpeed(350);
		//m_stepper.setSpeed(1000);						// full trottle, making the stepper intentionally a bit less powerfull :)
		DEBUG_PRINTLN("calibration started");
		m_stepper.runToNewPosition(STEPPER_RANGE);		// hoist all the way. hopefully nothing will break :)
		//m_stepper.setSpeed(300);
		DEBUG_PRINTLN("releasing tension");
		m_stepper.runToNewPosition(STEPPER_RANGE - (STEPPER_RANGE / 100));	// back down a bit to release tension

														
		m_stepper.setCurrentPosition(0);				// at start position. calibrate here
		m_stepper.setMaxSpeed(350.0);
		m_stepper.setAcceleration(200.0);
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
		DEBUG_PRINT("STEPPER_RANGE: "); DEBUG_PRINTLN(STEPPER_RANGE);

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
			setTargetRaw(-STEPPER_RANGE * (target / 100.0));
			//setTargetRaw(-target);
		}
		else
		{
			DEBUG_PRINTLN("Warning: not calibrated, no movement possible");
		}
	}

	bool stop()
	{
		if (!m_stopping)
		{
			m_stepper.stop();
			m_stopping = true;
			DEBUG_PRINTLN("START STOPPING");
		}
		else if (isAtTarget())
		{
			m_stopping = false;
			DEBUG_PRINTLN("HAS STOPPED");
			return true;
		}
		return false;
	}

	bool isAtTarget()
	{
		if (m_stepper.distanceToGo() == 0) 
			return true;
		return false;
	}

	long getPosition()
	{
		return m_stepper.currentPosition();
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
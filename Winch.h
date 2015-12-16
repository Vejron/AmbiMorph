class Winch
{
private:
	volatile bool m_calibrated = false;
	volatile int m_target;
	volatile int m_current;
	AccelStepper m_stepper = AccelStepper(HALFSTEP, MOTORPIN4, MOTORPIN2, MOTORPIN3, MOTORPIN1);

public:
	void begin()
	{
		pinMode(STEPPER_END_PIN, INPUT);
		pinMode(STEPPER_END_PIN, INPUT_PULLUP);
		GoToHomeSynch();
	}

	void GoToHomeSynch()
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

		// At endstop. Calibrate
		m_stepper.setCurrentPosition(0);

		// set default values
		m_stepper.setMaxSpeed(800.0);
		m_stepper.setAcceleration(500.0);
		m_stepper.disableOutputs();

		m_calibrated = true;
	}

	void setTarget(int target)
	{
		int actual = -STEPPER_MIN * (target / 100.0);
		setTargetRaw(actual);
	}

	void setTargetRaw(int target)
	{
		//noInterrupts();
		if (m_calibrated && target >= -STEPPER_MIN && target <= 0) // Has to be calibrated and within limits
		{
			m_target = target;
			m_stepper.enableOutputs();
			m_stepper.moveTo(m_target);
		}
		else
		{
			DEBUG_PRINTLN("Error: not in range");
		}
		//interrupts();
	}

	/// To be called from interrupt at least ? times per second
	void run()
	{
		if (!digitalRead(STEPPER_END_PIN))
		{
			m_calibrated = false;
			//Todo: out of calibration
			DEBUG_PRINTLN("Error: out of calibration, restart");
		}

		if (m_calibrated)
		{
			if (!m_stepper.run())
			{
				//at target, disable motor to make it less hot
				m_stepper.disableOutputs();
			}
		}
	}
};
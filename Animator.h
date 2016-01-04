#pragma once

elapsedMillis g_sinceLastStep;

class TwinkleFx
{
public:
	void run(CRGB& color, uint32_t timeStep)
	{
		if (g_sinceLastStep >= timeStep)
		{
			g_sinceLastStep = 0;
			if (_twinkrate < NUM_LEDS_PER_STRIP)
				_twinkrate = NUM_LEDS_PER_STRIP;             // Makes sure the twinkrate will cover ALL of the LED's as it's used as the maximum LED index value.
			int i = random16(_twinkrate);													      // A random number based on twinkrate. Higher number => fewer twinkles.

			if (i < NUM_LEDS_PER_STRIP)
				leds[i] = color; // Only the lowest probability twinkles will do. You could even randomize the hue/saturation.
			for (int j = 0; j < NUM_LEDS_PER_STRIP; j++)
				leds[j].fadeToBlackBy(_fade); // Use the FastLED fade method.
			LEDS.show();
		}
	}

private:
	int      _twinkrate = 100;                                     // The higher the value, the lower the number of twinkles.
	uint8_t  _fade = 4;                                     // How quickly does it fade? Lower = slower fade rate.
};

class RainBowFX
{
public:
	void run(Bulb *bulb, uint32_t timeStep)
	{
		if (g_sinceLastStep >= timeStep)
		{
			g_sinceLastStep = 0;
			//CHSV hsv = bulb->getHSV();
			//hsv.hue++;
			//bulb->setHSV(hsv);
		}
	}
};

class AlarmFX1
{
public:
	void run(CRGB& color, uint32_t timeStep)
	{
		if (g_sinceLastStep >= timeStep)
		{
			g_sinceLastStep = 0;

			for (size_t i = _stripIndex * _stripLength; i < _stripIndex * _stripLength + _stripLength; i++)
			{
				leds[i] = color;
			}

			_stripIndex++;
			if (_stripIndex >= _nbrOfStrips)
				_stripIndex = 0;
			for (int j = 0; j < NUM_LEDS_PER_STRIP; j++)
				leds[j].fadeToBlackBy(_fade); // Use the FastLED fade method.
			LEDS.show();
		}

	}
private:
	const size_t _nbrOfStrips = 4;
	const size_t _stripLength = 10;
	volatile size_t _stripIndex = 0;
	uint8_t  _fade = 40;
};

class AlarmFX2
{
public:
	void run(CRGB& color, uint32_t timeStep)
	{
		if (g_sinceLastStep >= timeStep)
		{
			g_sinceLastStep = 0;

			int j = (_nbrOfStrips - (_stripIndex + 1)) * _stripLength;
			int start = _stripIndex * _stripLength;
			int end = _stripIndex * _stripLength + _stripLength;
			for (int i = start; i < end; i++, j++)
			{
				// 10 - 0 + 1 = 9 * 50 = 45
				// 10 - 9 + 1 = 
				// 1 * 5... 5 6 7 8 9
				leds[i] = color;
				leds[j] = CRGB::White;
				//leds[]
			}

			_stripIndex++;
			if (_stripIndex >= _nbrOfStrips)
				_stripIndex = 0;
			for (size_t i = 0; i < NUM_LEDS_PER_STRIP; i++)
				leds[i].fadeToBlackBy(_fade);
			LEDS.show();
		}

	}
private:
	const int _nbrOfStrips = 4;
	const int _stripLength = 10;
	volatile int _stripIndex = 0;
	uint8_t  _fade = 80;
};





class Animator
{
public:

	void begin(Bulb *pBulb, Winch *pWinch, WifiController *pWifi);
	void run();

private:
	void next();

	WifiController *_pWifi;
	Bulb *_pBulb;
	Winch *_pWinch;

	TwinkleFx twinkleFX;
	RainBowFX rainBowFX;
	AlarmFX1 alarmFX1;
	AlarmFX2 alarmFX2;

	ControllState _ctrlState = ControllState::NONE;
	KeyFrame _keyFrames[MAX_KEYFRAMES];
	size_t _animationIndex = 0;
	size_t _animationLength = 0;
	volatile bool _loop = false;

	elapsedMillis _sinceLast;

};

void Animator::begin(Bulb *pBulb, Winch *pWinch, WifiController *pWifi)
{
	_pBulb = pBulb;
	_pWinch = pWinch;
	_pWifi = pWifi;
}

// only called from isr!!
void Animator::run()
{
	if (_pWifi->isMsgReady())	// was there a new command?
	{
		//transfer all frames
		_animationLength = _pWifi->copyFrames(_keyFrames);
		_loop = _pWifi->loop();

		//reset animation and timer
		_animationIndex = 0;
		_sinceLast = 0;
		_ctrlState = _keyFrames[0].state;
	}

	switch (_ctrlState)
	{
	case NONE:
		break;

	case DIRECT:
		// set values
		_pBulb->setRGB(_keyFrames[_animationIndex].color);
		_pWinch->setTarget(_keyFrames[_animationIndex].position, MAX_SPEED, MAX_ACCELERATION);
		_ctrlState = DIRECT_RUN;
		break;

	case DIRECT_RUN:
		next();
		break;

	case SMOOTH:
		// set values
		_pBulb->setRGB(_keyFrames[_animationIndex].color, _keyFrames[_animationIndex].timeOut, _keyFrames[_animationIndex].rate);
		_pWinch->setTarget(_keyFrames[_animationIndex].position, _keyFrames[_animationIndex].speed, _keyFrames[_animationIndex].acceleration);
		_ctrlState = SMOOTH_RUN;
		break;

	case SMOOTH_RUN:
		next();
		break;

	case FX_TWINKLE:
		_pWinch->setTarget(_keyFrames[_animationIndex].position, _keyFrames[_animationIndex].speed, _keyFrames[_animationIndex].acceleration);
		_ctrlState = FX_TWINKLE_RUN;
		break;

	case FX_TWINKLE_RUN:
		twinkleFX.run(_keyFrames[_animationIndex].color, _keyFrames[_animationIndex].rate);
		next();
		break;

	case FX_ALARM1:
		_pWinch->setTarget(_keyFrames[_animationIndex].position, _keyFrames[_animationIndex].speed, _keyFrames[_animationIndex].acceleration);
		_ctrlState = FX_ALARM1_RUN;
		break;

	case FX_ALARM1_RUN:
		alarmFX1.run(_keyFrames[_animationIndex].color, _keyFrames[_animationIndex].rate);
		next();
		break;

	case FX_ALARM2:
		_pWinch->setTarget(_keyFrames[_animationIndex].position, _keyFrames[_animationIndex].speed, _keyFrames[_animationIndex].acceleration);
		_ctrlState = FX_ALARM2_RUN;
		break;

	case FX_ALARM2_RUN:
		alarmFX2.run(_keyFrames[_animationIndex].color, _keyFrames[_animationIndex].rate);
		next();
		break;

	default:
		break;
	}

	//switch (_ctrlState)
	//{
	//case NONE:
	//	break;

	//case DIRECT:
	//	// set values
	//	_pBulb->setRGB(_keyFrames[_animationIndex].color);
	//	_pWinch->setTarget(_keyFrames[_animationIndex].position);
	//	next();
	//	break;

	//case SMOOTH:
	//	// set values
	//	_pBulb->setRgbSmooth(_keyFrames[_animationIndex].color, _keyFrames[_animationIndex].timeOut);
	//	_pWinch->setTarget(_keyFrames[_animationIndex].position);
	//	next();
	//	break;

	//case FX_TWINKLE:
	//	twinkleFX.run(_keyFrames[_animationIndex].color, _keyFrames[_animationIndex].rate);
	//	_pWinch->setTarget(_keyFrames[_animationIndex].position);
	//	next();
	//	break;

	//case FX_ALARM1:
	//	alarmFX1.run(_keyFrames[_animationIndex].color, _keyFrames[_animationIndex].rate);
	//	_pWinch->setTarget(_keyFrames[_animationIndex].position);
	//	next();
	//	break;

	//case FX_ALARM2:
	//	alarmFX2.run(_keyFrames[_animationIndex].color, _keyFrames[_animationIndex].rate);
	//	_pWinch->setTarget(_keyFrames[_animationIndex].position);
	//	next();
	//	break;

	//case FX_RAINBOW:
	//	rainBowFX.run(_pBulb, 20);
	//	_pWinch->setTarget(_keyFrames[_animationIndex].position);
	//	next();
	//	break;

	//default:
	//	break;
	//}
}

inline void Animator::next()
{
	// timeout
	if (_sinceLast >= _keyFrames[_animationIndex].timeOut)
	{
		//check if motor and light finished
		if (_pWinch->isAtTarget() && _pBulb->isAtTarget())
		{
			_sinceLast = 0;
			if (_animationIndex < _animationLength - 1)
			{
				_animationIndex++;
				_ctrlState = _keyFrames[_animationIndex].state;
				DEBUG_PRINTLN("NEXT");
			}
			else if (_loop)
			{
				//reset animation and timer
				_animationIndex = 0;
				_sinceLast = 0;
				_ctrlState = _keyFrames[0].state;
				DEBUG_PRINTLN("LOOP");
			}
			else
			{
				_ctrlState = ControllState::NONE;
				DEBUG_PRINTLN("NOLOOP");
			}
		}
	}
}

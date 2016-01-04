#pragma once

class Bulb
{
private:
	//volatile uint32_t _target = 0;
	//volatile uint32_t _current = 0;
	CRGB _currentRGB = CRGB(0, 0, 0);
	CRGB _targetRGB = CRGB(0, 0, 0);
	CRGB _oldRGB = CRGB(0, 0, 0);

	elapsedMillis _sinceLast;
	int _steps = 0;
	int _count = 0;
	int _rate = 10;

	enum BulbState
	{
		NONE,
		DIRECT,
		SMOOTH,
	};
	BulbState state = BulbState::NONE;

public:
	Bulb()
	{

	}

	void begin()
	{
		FastLED.setDither(0);
		LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
		LEDS.setBrightness(MAX_BRIGHTNESS);
		//fill_gradient_HSV(leds, NUM_LEDS_PER_STRIP, CHSV(0, 255, 255), CHSV(96, 255, 255));
		//LEDS.show();
		LEDS.showColor(_currentRGB);
		delay(2000);
		//setHSV(CHSV(42, 128, 128));
		//delay(1000);
	}

	bool isAtTarget()
	{
		if (_currentRGB == _targetRGB) return true;
		else return false;
	}

	CRGB getRGB()
	{
		return _currentRGB;
	}

	void setRGB(CRGB color)
	{
		_currentRGB = _targetRGB = color;
		state = BulbState::DIRECT;
		/*if (color != _currentRGB)
		{
			_currentRGB = color;
			_targetRGB = color;
			//CRGB rgb;
			//hsv2rgb_rainbow(_currentHSV, rgb);
			//fill_solid(leds, NUM_LEDS_PER_STRIP, rgb);
			LEDS.showColor(_currentRGB);
		}*/

	}

	void setRGB(CRGB color, uint32_t time, uint32_t rate)
	{
		_targetRGB = color;
		_oldRGB = _currentRGB;
		_steps = time / rate;
		_rate = rate;
		_count = 0;
		_sinceLast = 0;

		state = BulbState::SMOOTH;
	}

	void run() 
	{
		switch (state)
		{
		case Bulb::NONE:
			break;

		case Bulb::DIRECT:
			LEDS.showColor(_currentRGB);
			state = BulbState::NONE;
			break;

		case Bulb::SMOOTH:
			if (_sinceLast >= _rate)
			{
				_sinceLast = 0;
				_currentRGB = blend(_oldRGB, _targetRGB, (fract8)((_count++ / (float)_steps) * 255.0));
				if (_count >= _steps)
				{
					_currentRGB = _targetRGB;
					state = BulbState::NONE;
				}
				LEDS.showColor(_currentRGB);
			}
			break;

		default:
			break;
		}
	}

	/*void setRgbSmooth(CRGB targetColor, volatile uint32_t time)
	{
		volatile static bool _sRunning = false;
		static CRGB _sOldRGB;
		volatile static uint32_t _sTimeStep;
		volatile static fract8 _sFract;

		if (targetColor != _targetRGB)
		{
			_targetRGB = targetColor;
			_sOldRGB = _currentRGB;

			_sTimeStep = time / 256;
			if (_sTimeStep < 5)
				_sTimeStep = 5;
			_sFract = 0;
			_sRunning = true;
			DEBUG_PRINTLN(" Start of smoothing");
		}

		if (_sRunning)
		{
			if (_sinceLast >= _sTimeStep)
			{
				_sinceLast = 0;
				_currentRGB = blend(_sOldRGB, _targetRGB, _sFract++);

				//CRGB rgb;
				//hsv2rgb_rainbow(_currentHSV, rgb);
				LEDS.showColor(_currentRGB);

				if (_sFract == 255)
				{
					_sRunning = false;
					DEBUG_PRINTLN(" End of smmothing");
				}
			}
		}
	}*/
};

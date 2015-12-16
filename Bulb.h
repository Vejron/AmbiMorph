class Bulb
{
private:
	//volatile uint32_t _target = 0;
	//volatile uint32_t _current = 0;
	CRGB _currentRGB = CRGB(0, 0, 0);
	CRGB _targetRGB = CRGB(0, 0, 0);

	elapsedMillis _sinceLast;

public:
	Bulb()
	{

	}

	/*void step()
	{
	const int length = 50;

	for (size_t i = 0; i < NUM_LEDS_PER_STRIP; i++)
	{

	}
	}*/

	void begin()
	{
		LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
		LEDS.setBrightness(MAX_BRIGHTNESS);
		//fill_gradient_HSV(leds, NUM_LEDS_PER_STRIP, CHSV(0, 255, 255), CHSV(96, 255, 255));
		//LEDS.show();
		LEDS.showColor(_currentRGB);
		delay(2000);
		//setHSV(CHSV(42, 128, 128));
		//delay(1000);
	}

	CRGB getRGB()
	{
		return _currentRGB;
	}

	void setRGB(CRGB color)
	{
		if (color != _currentRGB)
		{
			_currentRGB = color;
			_targetRGB = color;
			//CRGB rgb;
			//hsv2rgb_rainbow(_currentHSV, rgb);
			//fill_solid(leds, NUM_LEDS_PER_STRIP, rgb);
			LEDS.showColor(_currentRGB);
		}

	}

	void setRgbSmooth(CRGB targetColor, volatile uint32_t time)
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
	}
};

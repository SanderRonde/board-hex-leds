#include <leds.h>
#include <FastLED.h>

CRGB colors[] = {CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Purple};
const int NUM_COLORS = sizeof(colors) / sizeof(CRGB);

namespace Leds
{
	CRGB leds[NUM_LEDS] = {CRGB::Black};

	void setup()
	{
		FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS, 0);
	}

	int offset = 0;
	void loop()
	{
		int color_idx = offset++ % NUM_COLORS;
		for (int i = 0; i < NUM_LEDS; i++)
		{
			leds[i] = colors[color_idx];

			color_idx = (++color_idx) % NUM_COLORS;
		}

		FastLED.show();
	}
}
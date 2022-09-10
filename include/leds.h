#pragma once

#include <Arduino.a.h>
#include <FastLED.a.h>

// For me: on desktop this should be D1, on server D5, don't ask me why
#define LED_PIN D5
#define NUM_LEDS 275

namespace Leds
{
	extern CRGB leds[NUM_LEDS];

	void setup();

	void loop();
}
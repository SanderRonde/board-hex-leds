#pragma once

#include <Arduino.h>
#include <FastLED.h>

// For me: on desktop this should be D1, on server D5, don't ask me why
#define LED_PIN D1
// We have 500 in reality but don't use the last 10.
// To prevent stuck pixels we just act like they don't exist.
#define NUM_LEDS 489

namespace Leds {
	extern CRGB leds[NUM_LEDS];

	void setup();

	void loop();
}
#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"

// We say D1 but in actuality the magic happens on pin 5
#define LED_PIN D1
// #define NUM_LEDS 275
#define NUM_LEDS 500

namespace Leds {
	extern CRGB leds[NUM_LEDS];

	void setup();

	void loop();
}
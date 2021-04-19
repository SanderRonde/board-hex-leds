#pragma once

#include <Arduino.h>
#include <FastLED.h>

// We say D1 but in actuality the magic happens on pin 5
#define LED_PIN D1
#define LEDS_PER_M 60
#define METERS_PER_STRIP 5
#define LEDS_PER_STRIP (LEDS_PER_M * METERS_PER_STRIP)
#define STRIPS 1
#define NUM_LEDS (LEDS_PER_STRIP * STRIPS)

namespace Leds {
	extern CRGB leds[NUM_LEDS];

	void setup();

	void loop();
}
#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "hex.h"

// We say D1 but in actuality the magic happens on pin 5
#define LED_PIN D1

namespace Leds
{
	extern CRGB leds[NUM_LEDS];

	void setup();

	void loop();
}
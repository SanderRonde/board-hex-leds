#pragma once

#include <Arduino.h>
#include <string.h>
#include <FastLED.h>

namespace Util
{
	char **split_string(String str);
	void free_split(char **strings);
	double divide(int val1, int val2);
	CRGB fade_towards_color(CRGB cur, const CRGB target, uint8_t amount);
	CHSV fade_towards_color(CHSV cur, const CHSV target, uint8_t amount);
	CRGB fade_towards_color(CRGB cur, const CRGB target, float amount);
	CHSV fade_towards_color(CHSV cur, const CHSV target, float amount);
	CRGB fade_towards_color(CRGB cur, const CRGB target, double amount);
	CHSV fade_towards_color(CHSV cur, const CHSV target, double amount);

	long random_except_range(long min, long max, long except_start, long except_end);
}
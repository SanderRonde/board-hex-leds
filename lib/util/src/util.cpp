#include <Arduino.h>
#include <string.h>
#include <FastLED.h>

namespace Util
{
	const int SPLIT_LEN = 10;
	char **split_string(String str)
	{
		char **strings = (char **)malloc(sizeof(char *) * SPLIT_LEN);
		for (int i = 0; i < SPLIT_LEN; i++)
		{
			strings[i] = NULL;
		}

		int str_index = 0;
		const char *delim = {" "};
		char *copy = (char *)malloc(sizeof(char) * str.length());
		strcpy(copy, str.c_str());
		char *ptr = strtok(copy, delim);
		while (ptr != NULL)
		{
			strings[str_index++] = strdup(ptr);
			ptr = strtok(NULL, delim);
		}
		free(copy);

		return strings;
	}

	void free_split(char **strings)
	{
		for (int i = 0; i < SPLIT_LEN; i++)
		{
			if (strings[i] != NULL)
			{
				free(strings[i]);
			}
		}
		free(strings);
	}

	double divide(int val1, int val2)
	{
		return (double)val1 / (double)val2;
	}

	// Helper function that blends one uint8_t toward another by a given amount
	uint8_t n_blend_u8_towards_u8(uint8_t cur, const uint8_t target, uint8_t amount)
	{
		if (cur == target)
			return cur;

		if (cur < target)
		{
			uint8_t delta = target - cur;
			delta = scale8_video(delta, amount);
			return cur + delta;
		}
		else
		{
			uint8_t delta = cur - target;
			delta = scale8_video(delta, amount);
			return cur - delta;
		}
	}

	// Blend one CRGB color toward another CRGB color by a given amount.
	// Blending is linear, and done in the RGB color space.
	// This function modifies 'cur' in place.
	CRGB fade_towards_color(CRGB cur, const CRGB target, uint8_t amount)
	{
		cur.red = n_blend_u8_towards_u8(cur.red, target.red, amount);
		cur.green = n_blend_u8_towards_u8(cur.green, target.green, amount);
		cur.blue = n_blend_u8_towards_u8(cur.blue, target.blue, amount);
		return cur;
	}

	long random_except_range(long min, long max, long except_start, long except_end)
	{
		long except_size = except_end - except_start;
		long random_num = random(min, max - except_size);
		if (random_num >= except_start)
		{
			return random_num + except_size;
		}
		return random_num;
	}
}
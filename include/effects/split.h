#pragma once

#include "effects.h"
#include "constants.h"
#include "util.h"
#include "change.h"

typedef struct split_color
{
	Hex *hex;
	change_t first_half_color;
	change_t second_half_color;
	int split_led_idx;
	bool uses_split;
} split_color_t;

namespace Split
{
	split_color_t set_new_color_target(split_color_t prev_split, int interval_min, int interval_max, Hex *hex, bool use_split, bool is_first = false);
	void bump_progress(split_color_t *split_color, int progress_amount);
	split_color_t change_on_total_reached(split_color_t split_color, int interval_min, int interval_max);
	bool is_pixel_in_half(int led_idx, int half_start, int total_leds);
	int get_current_value(split_color_t split_color, int led_idx);
	CRGB get_current_color(split_color_t split_color, int led_idx);
}
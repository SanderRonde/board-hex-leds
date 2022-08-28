#include "effects.h"
#include "constants.h"
#include "util.h"
#include "effects/split.h"

namespace Split
{
	split_color_t set_new_color_target(split_color_t prev_split, int interval_min, int interval_max, HexNS::Hex *hex, bool use_split, bool is_first)
	{
		split_color_t split_color;
		split_color.uses_split = use_split;
		split_color.hex = hex;
		split_color.first_half_color.current = prev_split.first_half_color.next;
		split_color.first_half_color.progress = 0;
		split_color.first_half_color.total = random(interval_min, interval_max);
		split_color.first_half_color.next = random(0, MAX_CSHV_VALUE);
		if (use_split)
		{
			split_color.second_half_color.current = prev_split.second_half_color.next;
			split_color.second_half_color.progress = 0;
			split_color.second_half_color.total = split_color.first_half_color.total;
			split_color.second_half_color.next = Util::random_except_range(0, MAX_CSHV_VALUE, max(split_color.first_half_color.next - MIN_SPLIT_COLOR_DIFF, 0), min(split_color.first_half_color.next + MIN_SPLIT_COLOR_DIFF, MAX_CSHV_VALUE));
			split_color.split_led_idx = prev_split.split_led_idx;
			if (is_first)
			{
				split_color.split_led_idx = random(0, hex->num_leds);
			}
		}
		return split_color;
	}

	void bump_progress(split_color_t *split_color, int progress_amount)
	{
		split_color->first_half_color.progress += progress_amount;
		if (split_color->uses_split)
		{
			split_color->second_half_color.progress += progress_amount;
		}
	}

	split_color_t change_on_total_reached(split_color_t split_color, int interval_min, int interval_max)
	{
		if (split_color.first_half_color.progress > split_color.first_half_color.total)
		{
			return set_new_color_target(split_color, interval_min, interval_max, split_color.hex, split_color.uses_split);
		}
		return split_color;
	}

	bool is_pixel_in_half(int led_idx, int half_start, int total_leds)
	{
		int half_num_leds = total_leds / 2;
		if (half_start + half_num_leds > total_leds)
		{
			return led_idx > half_start || led_idx <= ((half_start + half_num_leds) % total_leds);
		}
		return led_idx > half_start && led_idx <= half_start + half_num_leds;
	}

	int get_current_value(split_color_t split_color, int led_idx)
	{
		change_t cur_change = split_color.uses_split && is_pixel_in_half(led_idx, split_color.split_led_idx, split_color.hex->num_leds) ? split_color.second_half_color : split_color.first_half_color;
		int diff = cur_change.next - cur_change.current;
		double progress = Util::divide(cur_change.progress, cur_change.total);
		return cur_change.current + (diff * progress);
	}

	CRGB get_current_color(split_color_t split_color, int led_idx)
	{
		change_t cur_change = split_color.uses_split && is_pixel_in_half(led_idx, split_color.split_led_idx, split_color.hex->num_leds) ? split_color.second_half_color : split_color.first_half_color;
		CRGB current = CHSV(cur_change.current, MAX_CSHV_VALUE, MAX_CSHV_VALUE);
		CRGB next = CHSV(cur_change.next, MAX_CSHV_VALUE, MAX_CSHV_VALUE);
		double progress = Util::divide(cur_change.progress, cur_change.total);
		return Util::fade_towards_color(current, next, progress * MAX_CSHV_VALUE);
	}
}
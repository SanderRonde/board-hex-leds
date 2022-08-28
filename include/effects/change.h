#pragma once

#include "effects.h"
#include "constants.h"
#include "util.h"

/**
 * This is the idea:
 *
 * Have a "background" that is filled with some low intensity
 * random colors.
 *
 * Have an X number of hexes turn into a high intensity color.
 * This color changes gradually over time. It also moves
 * from one hex to one next to it.
 */
typedef struct change
{
	int current;
	int next;
	int progress;
	int total;
} change_t;

namespace Change
{
	change_t set_new_color_target(change_t prev_change, int interval_min, int interval_max);
	int get_current_value(change_t change);
	CRGB get_current_color(change_t change);
}
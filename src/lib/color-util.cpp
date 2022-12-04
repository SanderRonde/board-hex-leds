#include "color-util.h"

CRGB fade_towards_color(CRGB from, CRGB to, float amount)
{
	return CRGB(
			floor(from.r * (1 - amount) + (to.r * amount)),
			floor(from.g * (1 - amount) + (to.g * amount)),
			floor(from.b * (1 - amount) + (to.b * amount)));
}
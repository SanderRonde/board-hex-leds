#include "config.h"
#include "effects.h"
#include "constants.h"
#include "hex.h"
#include "telnet.h"
#include "util.h"
#include "leds.h"
#include "api.h"
#include "effects/FireSparkles.h"
#include "effects/EffectBase.h"
#include "color-util.h"

const CRGB FIRE_RED = CRGB(255, 0, 0);
const int NUM_CREATED_SPARKS = 5;
const int FADE_STEP = 15;

spark_t create_spark(Hexes *hexes)
{
	return {
			.empty = false,
			.led_index = (uint16_t)random(0, NUM_LEDS),
			.progress = 0,
			.past_peak = false};
}

spark_t loop_spark(spark_t spark, Hexes *hexes)
{
	if (!spark.past_peak)
	{
		spark.progress = min((int)spark.progress + FADE_STEP, 255);
		if (spark.progress >= 255)
		{
			spark.past_peak = true;
		}
	}
	else
	{
		spark.progress = max((int)spark.progress - FADE_STEP, 0);
		if (spark.progress <= 0)
		{
			return {
					.empty = true};
		}
	}
	return spark;
}

// TODO: should require RequestObj and get its params from that
FireSparkles::FireSparkles(Hexes *hexes) : EffectBase()
{
	for (int i = 0; i < MAX_NUM_SPARKS; i++)
	{
		_sparks[i] = {
				.empty = true};
	}
}

/**
 * The idea: have the bottom-most row be superhot (yellow/white)
 * and occasionally have a "spark" trickle up. It just follows
 * the LEDs up.
 * Maybe: sparks are entire panels at once.
 * Maybe: have it make a choice at every split on what direction to go in
 */

bool FireSparkles::loop(Hexes *hexes)
{
	for (size_t i = 0; i < hexes->num_hexes; i++)
	{
		hexes->get_by_index(i)->set_color(FIRE_RED);
	}

	if (random(0, 1) == 0)
	{
		for (int i = 0; i < NUM_CREATED_SPARKS; i++)
		{
			// Find free index
			for (size_t i = 0; i < sizeof(_sparks) / sizeof(spark_t); i++)
			{
				if (_sparks[i].empty)
				{
					_sparks[i] = create_spark(hexes);
					break;
				}
			}
		}
	}

	// Draw sparks
	for (size_t i = 0; i < MAX_NUM_SPARKS; i++)
	{
		auto spark = _sparks[i];
		if (spark.empty)
		{
			continue;
		}
		float progress = Util::divide(spark.progress, 255);
		float inverted = 1 - progress;
		inverted *= inverted;
		progress = 1 - inverted;
		hexes->set_led_at_index(spark.led_index, fade_towards_color(CRGB(255, 0, 0), CRGB(255, 30, 0), progress));
	}

	// Move sparks forward
	for (int i = 0; i < MAX_NUM_SPARKS; i++)
	{
		auto spark = _sparks[i];
		if (spark.empty)
		{
			continue;
		}
		_sparks[i] = loop_spark(spark, hexes);
	}
	return true;
}
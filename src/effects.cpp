#include <hex.h>
#include <leds.h>
#include <string.h>
#include <FastLED.h>
#include <util.h>
#include <telnet.h>
#include "constants.h"
#include "effects.h"
#include "effects/Rainbow.h"
#include "effects/Fire.h"
#include "effects/FireSparkles.h"
#include "effects/EdgeRainbow.h"
#include "effects/MoveAround.h"
#include "effects/RandomColors.h"
#include "effects/RandomColorsGradual.h"
#include "effects/Fade.h"
#include "effects/change.h"
#include "api.h"

namespace Effects
{
	void enable();

	bool animating = false;
	EffectBase *effect = NULL;
	Hexes *hexes;

	void set_effect(EffectBase *new_effect)
	{
		if (effect)
		{
			free(effect);
		}
		effect = new_effect;
		if (new_effect != NULL)
		{
			animating = true;
		}
		else
		{
			animating = false;
		}
		enable();
	}

	void stop_animation()
	{
		set_effect(NULL);
	}

	namespace Effects
	{
		void set_led(RequestObj *request)
		{
			enable();
			stop_animation();
			hexes->set_led_at_index(request->intv("index"), request->colorv("color"));
			FastLED.show();
		}

		void set_led_in_hex(RequestObj *request)
		{
			enable();
			stop_animation();
			Hex *hex = hexes->get_by_id(request->intv("hex_id"));
			hex->set_at_index(request->intv("index"), request->colorv("color"));
			FastLED.show();
		}

		void set_hex(RequestObj *request)
		{
			enable();
			stop_animation();
			Hex *hex = hexes->get_by_id(request->intv("hex_id"));
			hex->set_color(request->colorv("color"));
			FastLED.show();
		}

		void set_rainbow_hex(RequestObj *request)
		{
			// For debugging
			enable();
			stop_animation();
			for (size_t i = 0; i < hexes->num_hexes; i++)
			{
				hexes->get_by_index(i)->set_color(CRGB::Black);
			}
			Hex *hex = hexes->get_by_id(request->intv("hex_id"));
			for (size_t i = 0; i < hex->num_leds; i++) {
				float percentage = (float)i / (float)hex->num_leds;
				hex->set_at_index(i, CHSV(floor(percentage * 255.0f), 255, 255));
			}
			FastLED.show();
		}

		void set_all(RequestObj *request)
		{
			enable();
			stop_animation();
			CRGB color = request->colorv("color");
			for (size_t i = 0; i < hexes->num_hexes; i++)
			{
				hexes->get_by_index(i)->set_color(color);
			}
			FastLED.show();
		}

		void enable_rainbow(RequestObj *request)
		{
			set_effect(new Rainbow(request));
		}

		void enable_fire()
		{
			set_effect(new Fire(hexes));
		}

		void enable_fire_sparkles()
		{
			set_effect(new FireSparkles(hexes));
		}

		void enable_edge_rainbow(RequestObj *request)
		{
			set_effect(new EdgeRainbow(request));
		}

		void move_around()
		{
			set_effect(new MoveAround(hexes));
		}

		void random_colors_gradual(RequestObj *request)
		{
			set_effect(new RandomColorsGradual(hexes, request));
		}

		void random_colors(RequestObj *request)
		{
			set_effect(new RandomColors(request));
		}

		void fade(RequestObj *request)
		{
			set_effect(new FadeFromReq(hexes, request));
		}
	}

	bool enabled = true;
	bool enabling = false;
	bool disabling = false;
	int state_change_progress = MAX_CSHV_VALUE;
	unsigned long state_change_start = millis();
	void enable()
	{
		if (enabled)
			return;

		if (ENABLE_FADE_OUT)
		{
			enabled = true;
			enabling = true;
			state_change_progress = 0;
			state_change_start = millis();
		}
		else
		{
			enabled = true;
			enabling = false;
		}
	}

	void disable()
	{
		if (ENABLE_FADE_OUT)
		{
			disabling = true;
			state_change_progress = MAX_CSHV_VALUE;
			state_change_start = millis();
		}
		else
		{
			enabled = false;
			disabling = false;
			FastLED.showColor(CRGB::Black);
		}
	}

	bool is_enabled()
	{
		return enabled;
	}

	void setup(Hexes *hexes_)
	{
		hexes = hexes_;
	}

	void loop()
	{
		if (animating && enabled && effect)
		{
			bool draw = effect->loop(hexes);
			yield();

			if (enabling || disabling)
			{
				unsigned long time_passed = millis() - state_change_start;
				double percentage_transition_complete = Util::divide(time_passed, STATE_CHANGE_TIME);
				if (enabling)
				{
					state_change_progress = percentage_transition_complete * MAX_CSHV_VALUE;
					if (time_passed >= STATE_CHANGE_TIME)
					{
						state_change_progress = MAX_CSHV_VALUE;
						enabling = false;
					}
				}
				else if (disabling)
				{
					state_change_progress = MAX_CSHV_VALUE - (percentage_transition_complete * MAX_CSHV_VALUE);
					if (time_passed >= STATE_CHANGE_TIME)
					{
						state_change_progress = 0;
						disabling = false;
						enabled = false;
						FastLED.showColor(CRGB::Black);
						return;
					}
				}

				if (state_change_progress != MAX_CSHV_VALUE)
				{
					FastLED.show(state_change_progress);
					return;
				}
			}

			if (draw) {
				FastLED.show();
			}
		}
	}
}
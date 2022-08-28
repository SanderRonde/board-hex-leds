#include <hex.h>
#include <leds.h>
#include <string.h>
#include <FastLED.h>
#include <util.h>
#include <telnet.h>
#include "constants.h"
#include "effects.h"
#include "effects/Rainbow.h"
#include "effects/EdgeRainbow.h"
#include "effects/MoveAround.h"
#include "effects/RandomColors.h"
#include "effects/RandomColorsGradual.h"
#include "effects/change.h"
#include "api.h"

namespace Effects
{
	void enable();

	bool animating = false;
	EffectBase *effect = NULL;

	void set_effect(EffectBase *new_effect)
	{
		if (effect)
		{
			free(effect);
		}
		effect = new_effect;
	}

	void stop_animation()
	{
		animating = false;
		set_effect(NULL);
	}

	namespace Effects
	{
		void set_led(RequestObj *request)
		{
			enable();
			animating = false;
			set_effect(NULL);
			Leds::leds[request->intv("index")] = request->colorv("color");
			FastLED.show();
		}

		void set_led_in_hex(RequestObj *request)
		{
			enable();
			animating = false;
			set_effect(NULL);
			HexNS::Hex *hex = HexNS::hexes->get_by_id(request->intv("hex_id"));
			hex->set_at_index(request->intv("index"), request->colorv("color"));
			FastLED.show();
		}

		void set_hex(RequestObj *request)
		{
			enable();
			animating = false;
			set_effect(NULL);
			HexNS::Hex *hex = HexNS::hexes->get_by_id(request->intv("hex_id"));
			hex->set_color(request->colorv("color"));
			FastLED.show();
		}

		void set_all(RequestObj *request)
		{
			enable();
			animating = false;
			set_effect(NULL);
			CRGB color = request->colorv("color");
			for (int i = 0; i < HexNS::hexes->num_hexes; i++)
			{
				HexNS::hexes->get_by_index(i)->set_color(color);
			}
			FastLED.show();
		}

		void enable_rainbow(RequestObj *request)
		{
			enable();
			set_effect(new Rainbow(request));
			animating = true;
		}

		void enable_edge_rainbow(RequestObj *request)
		{
			enable();
			set_effect(new EdgeRainbow(request));
			animating = true;
		}

		void move_around()
		{
			enable();
			set_effect(new MoveAround());
			animating = true;
		}

		void random_colors_gradual(RequestObj *request)
		{
			enable();
			set_effect(new RandomColorsGradual(request));
			animating = true;
		}

		void random_colors(RequestObj *request)
		{
			enable();
			set_effect(new RandomColors(request));
			animating = true;
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

	void loop()
	{
		if (animating && enabled && effect)
		{
			effect->loop();

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

			FastLED.show();
		}
	}
}
#pragma once
#include <FastLED.h>
#include "api.h"
#include "hex.h"

namespace Effects
{
	namespace Effects
	{
		void set_led(RequestObj *request);

		void set_led_in_hex(RequestObj *request);

		void set_hex(RequestObj *request);

		void set_rainbow_hex(RequestObj *request);

		void enable_rainbow(RequestObj *request);

		void enable_edge_rainbow(RequestObj *request);

		void set_all(RequestObj *request);

		void move_around();

		void random_colors_gradual(RequestObj *request);

		void random_colors(RequestObj *request);

		void fade(RequestObj *request);
	}

	void enable();

	void disable();

	bool is_enabled();

	void setup(Hexes* hexes);

	void loop();
}
#include <FastLED.h>

namespace Effects
{
	namespace Effects
	{
		void set_led(int index, CRGB color);

		void set_led_in_hex(int index, int hex_id, CRGB color);

		void set_hex(int hex_id, CRGB color);

		void enable_rainbow(int revolve_time);

		void enable_edge_rainbow(int revolve_time);

		void set_all(CRGB str_color);

		void move_around();

		void random_colors_gradual(int wait_time_min, int wait_time_max, int neighbour_influence, bool use_pastel, bool split);

		void random_colors(int wait_time);
	}

	void enable();

	void disable();

	bool is_enabled();

	void loop();
}
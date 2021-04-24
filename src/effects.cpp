#include <hex.h>
#include <leds.h>
#include <string.h>
#include <FastLED.h>
#include <util.h>
#include <telnet.h>

#define MAX_CSHV_VALUE 256
#define NUM_HIGH_INTENSITY_HEXES 2

#define BACKGROUND_COLOR_CHANGE_MIN 10000
#define BACKGROUND_COLOR_CHANGE_MAX 20000
#define MOVE_CHANGE_MIN 5000
#define MOVE_CHANGE_MAX 8000
#define BACKGROUND_SATURATION 160
#define BACKGROUND_LIGHTNESS 180
#define MAX_MOVE_TARGET_ITERATIONS 1000
#define PEAK_BRIGHTNESS_CENTER_DISTANCE 1
#define LOWEST_BRIGTHENESS_CENTER_DISTANCE 2
#define RAINBOW_STEP 15
#define NEIGHBOUR_INFLUENCE 255
#define STATE_CHANGE_TIME 500

#define MOVE_SAME_BACKGROUND 1

namespace Effects
{
	void enable();

	CRGB parse_color(String color)
	{
		CRGB color_obj;
		char red[3];
		char green[3];
		char blue[3];

		const char *color_c_str = color.c_str();
		strncpy(red, color_c_str + 1, 2);
		strncpy(green, color_c_str + 3, 2);
		strncpy(blue, color_c_str + 5, 2);
		red[2] = green[2] = blue[2] = '\n';
		color_obj.r = strtol(red, NULL, 16);
		color_obj.g = strtol(green, NULL, 16);
		color_obj.b = strtol(blue, NULL, 16);

		return color_obj;
	}

	bool animating = false;
	void (*animation_fn)();

	void stop_animation()
	{
		animating = false;
		animation_fn = NULL;
	}

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
		change_t set_new_color_target(change_t prev_change, int interval_min, int interval_max)
		{
			change_t change;
			change.current = prev_change.next;
			change.progress = 0;
			change.total = random(interval_min, interval_max);
			change.next = random(0, 255);
			return change;
		}

		int get_current_value(change_t change)
		{
			int diff = change.next - change.current;
			double progress = Util::divide(change.progress, change.total);
			return change.current + (diff * progress);
		}

		CRGB get_current_color(change_t change)
		{
			CRGB current = CHSV(change.current, 255, 255);
			CRGB next = CHSV(change.next, 255, 255);
			double progress = Util::divide(change.progress, change.total);
			return Util::fade_towards_color(current, next, progress * 255);
		}
	}

	namespace Animations
	{
		namespace Rainbow
		{
			long long last_iteration;
			double revolve_step;
			double offset;
			int num_hexes;

			void setup(int _revolve_time)
			{
				revolve_step = Util::divide(255, _revolve_time);
				last_iteration = 0;
				offset = 0;
			}

			void loop()
			{
				long long time_diff = millis() - last_iteration;
				double added_offset = (double)time_diff * revolve_step;
				offset = std::fmod(offset + added_offset, MAX_CSHV_VALUE);
				last_iteration = millis();

				int rounded_offset = (int)round(offset);

				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					HexNS::Hex *hex = HexNS::hexes->get_by_index(i);
					int hex_led_step = (int)Util::divide(MAX_CSHV_VALUE, hex->num_leds);

					int total_step = rounded_offset;
					for (int j = 0; j < hex->num_leds; j++, total_step = (hex_led_step + total_step) % MAX_CSHV_VALUE)
					{
						hex->set_at_index(j, CHSV(total_step, 255, 255));
					}
				}
			}
		}

		namespace EdgeRainbow
		{
			long long last_iteration;
			double revolve_step;
			double offset;
			int num_hexes;

			void setup(int _revolve_time)
			{
				revolve_step = Util::divide(255, _revolve_time);
				last_iteration = 0;
				offset = 0;
			}

			void loop()
			{
				long long time_diff = millis() - last_iteration;
				double added_offset = (double)time_diff * revolve_step;
				offset = std::fmod(offset + added_offset, MAX_CSHV_VALUE);
				last_iteration = millis();

				int rounded_offset = (int)round(offset);

				int total_offset = rounded_offset;
				for (int i = 0; i < NUM_LEDS; i++)
				{
					Leds::leds[i] = CHSV(total_offset, 255, 255);

					total_offset = (total_offset + RAINBOW_STEP) % MAX_CSHV_VALUE;
				}
			}
		}

		namespace RandomColorsGradual
		{
			int wait_time_min = 0;
			int wait_time_max = 0;
			uint8_t neighbour_influence = 0;
			bool use_pastel = false;
			long long last_iteration;
			long long last_refresh_iteration;
			change_t colors[HEXES_UPPERBOUND];

			void setup(int _wait_time_min, int _wait_time_max, int _neighbour_influence, bool _use_pastel)
			{
				wait_time_min = _wait_time_min;
				wait_time_max = _wait_time_max;
				neighbour_influence = _neighbour_influence;
				use_pastel = _use_pastel;
				for (int i = 0; i < HEXES_UPPERBOUND; i++)
				{
					change_t prev;
					prev.next = random(0, 255);
					colors[i] = Change::set_new_color_target(prev, wait_time_min, wait_time_max);
				}
				last_iteration = millis();
			}

			void loop()
			{
				int time_diff = millis() - last_iteration;
				last_iteration = millis();

				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					colors[i].progress += time_diff;
					if (colors[i].progress > colors[i].total)
					{
						colors[i] = Change::set_new_color_target(colors[i], wait_time_min, wait_time_max);
					}

					HexNS::Hex *hex = HexNS::hexes->get_by_index(i);
					CRGB current_color = use_pastel ? Change::get_current_color(colors[i]) : CHSV(Change::get_current_value(colors[i]), 255, 255);

					for (int j = 0; j < hex->num_leds; j++)
					{
						HexNS::Hex *neighbour = hex->get_neighbour_at_led(j);

						if (neighbour == NULL)
						{
							hex->set_at_index(j, current_color);
						}
						else
						{
							// Get the color of the neighbour
							CRGB neighbour_color = use_pastel ? Change::get_current_color(colors[neighbour->index]) : CHSV(Change::get_current_value(colors[neighbour->index]), 255, 255);

							// Move towards it a little
							hex->set_at_index(j, Util::fade_towards_color(current_color, neighbour_color, neighbour_influence));
						}
					}
				}
			}
		}

		namespace RandomColors
		{
			int wait_time = 0;
			long long last_iteration;

			void setup(int _wait_time)
			{
				wait_time = _wait_time;
			}

			void loop()
			{
				if (millis() - last_iteration <= wait_time)
				{
					return;
				}

				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					HexNS::Hex *hex = HexNS::hexes->get_by_index(i);

					hex->set_color(CHSV(random(0, 255), 255, 255));
				}

				last_iteration = millis();
			}
		}

		namespace MoveAround
		{

			typedef struct high_intensity_hex
			{
				int index;

				change_t move_change;
				HexNS::hex_side_t move_side;

				change_t color_change;
			} high_intensity_hex_t;

			change_t background_colors[HEXES_UPPERBOUND];

			high_intensity_hex_t high_intensity_hexes[NUM_HIGH_INTENSITY_HEXES];
			long long last_iteration;

			int random_except(int min, int max, int except)
			{
				int random_num = random(min, max);
				while (random_num == except)
				{
					random_num = random(min, max);
				}
				return random_num;
			}

			int choose_new_move_direction(int current_target)
			{
				int iterations = 0;

				HexNS::hex_side_t random_direction;
				while (true)
				{
					iterations++;

					if (iterations > MAX_MOVE_TARGET_ITERATIONS)
					{
						return -1;
					}
					random_direction = (HexNS::hex_side_t)random(0, HEX_SIDES);

					HexNS::Hex *target = HexNS::hexes->get_by_id(current_target)->get_neighbour(random_direction);

					// Check if the neighbour exists
					if (target == NULL)
						continue;
					// Check if they are not currently being animated towards or from
					for (int i = 0; i < NUM_HIGH_INTENSITY_HEXES; i++)
					{
						// Ignore the one we're currently choosing for
						if (high_intensity_hexes[i].move_change.next != current_target)
						{
							if (target->id == high_intensity_hexes[i].move_change.next ||
									target->id == high_intensity_hexes[i].move_change.current)
							{
								continue;
							}
						}
					}

					return random_direction;
				}
			}

			change_t set_new_move_target(int current_id)
			{
				change_t move_change;
				move_change.current = current_id;
				int move_direction = choose_new_move_direction(current_id);
				if (move_direction == -1)
				{
					move_change.next = current_id;
				}
				else
				{
					move_change.next = HexNS::hexes->get_by_id(current_id)->get_neighbour((HexNS::hex_side_t)move_direction)->id;
				}

				move_change.progress = 0;
				move_change.total = random(MOVE_CHANGE_MIN, MOVE_CHANGE_MAX);
				return move_change;
			}

			void setup()
			{
				change_t prev;
				prev.next = 0;
#ifdef MOVE_SAME_BACKGROUND
				background_colors[0] = Change::set_new_color_target(prev, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
#else
				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					background_colors[i] = set_new_color_target(prev, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
				}
#endif

				for (int i = 0; i < NUM_HIGH_INTENSITY_HEXES; i++)
				{
					high_intensity_hex_t *hex = &high_intensity_hexes[i];

					bool hex_taken = false;
					do
					{
						hex_taken = false;
						hex->index = random(0, HexNS::hexes->num_hexes);
						for (int j = 0; j < i; j++)
						{
							if (high_intensity_hexes[j].index == hex->index)
							{
								hex_taken = true;
								break;
							}
						}
					} while (hex_taken);
					LOGF("Choosing hex %d\n", hex->index);
					hex->color_change = Change::set_new_color_target(prev, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
					hex->move_side = (HexNS::hex_side_t)random(0, HEX_SIDES);
					change_t move_change;
					move_change.current = hex->index;
					move_change.next = random_except(0, HexNS::hexes->num_hexes, hex->index);
					move_change.progress = 0;
					move_change.total = random(MOVE_CHANGE_MIN, MOVE_CHANGE_MAX);
					hex->move_change = move_change;
				}

				last_iteration = millis();
			}

			int get_direction_angle(HexNS::hex_side_t direction)
			{
				return (direction * ANGLE_PER_SIDE) + (ANGLE_PER_SIDE / 2);
			}

			int mod_positive(int base, int modulo)
			{
				return base - floor(Util::divide(base, modulo)) * modulo;
			}

			/**
			 * Reference triangle
			* 		 ^ B
			* 	  /  \
			*   /     \
			* A/________\ C
			* 
			* Say that the hex center point is A, the center
			* of the color is at B and the LED is C.
			* 
			* We know AB (progress * 2R)
			* We know AC (R)
			* We know the angle of A (abs(center_point_angle - led_angle))
			* 
			* To calculate BC, we do the following
			* BC^2 = AB^2 + AC^2 - 2(AB)(AC)cos(angle(A))
			*/
			int get_distance_between_led_and_center(float center_progress, int center_angle, int led_angle)
			{
				int AB = center_progress * 2;
				int AC = 1;
				int A_angle = abs(mod_positive(center_angle - led_angle + 180, 360) - 180);

				int BC_squared = sq(AB) + sq(AC) - (2 * AB * AC * cos8(A_angle));
				return sqrt16(BC_squared);
			}

			void loop()
			{
				long long time_diff = millis() - last_iteration;
				last_iteration = millis();

				// Paint backgrounds
#ifndef MOVE_SAME_BACKGROUND
				for (int hex_idx = 0; hex_idx < HexNS::hexes->num_hexes; hex_idx++)
				{
#endif
#ifdef MOVE_SAME_BACKGROUND
					int hex_idx = 0;
#endif
					HexNS::Hex *hex = HexNS::hexes->get_by_index(hex_idx);
					background_colors[hex_idx].progress += time_diff;
					if (background_colors[hex_idx].progress > background_colors[hex_idx].total)
					{
						// Choose a new color target
						background_colors[hex_idx] = Change::set_new_color_target(background_colors[hex_idx], BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
					}

					// Find color
					int current_hue = Change::get_current_value(background_colors[hex_idx]);
#ifdef MOVE_SAME_BACKGROUND
					for (int i = 0; i < HexNS::hexes->num_hexes; i++)
					{
						hex = HexNS::hexes->get_by_index(i);
#endif
						hex->set_color(CHSV(current_hue, BACKGROUND_SATURATION, BACKGROUND_LIGHTNESS));
#ifdef MOVE_SAME_BACKGROUND
					}
#endif
#ifndef MOVE_SAME_BACKGROUND
				}
#endif

				for (int i = 0; i < NUM_HIGH_INTENSITY_HEXES; i++)
				{
					HexNS::Hex *hex = HexNS::hexes->get_by_index(i);

					// Do color change
					high_intensity_hexes[i].color_change.progress += time_diff;
					if (high_intensity_hexes[i].color_change.progress > high_intensity_hexes[i].color_change.total)
					{
						// Choose a new color target
						high_intensity_hexes[i].color_change = Change::set_new_color_target(high_intensity_hexes[i].color_change, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
					}

					// Do move change
					high_intensity_hexes[i].move_change.progress += time_diff;
					if (high_intensity_hexes[i].move_change.progress > high_intensity_hexes[i].move_change.total)
					{
						// Choose a new move target
						high_intensity_hexes[i].move_change = set_new_move_target(high_intensity_hexes[i].move_change.next);
					}

					// Do the coloring
					if (high_intensity_hexes[i].move_change.next == high_intensity_hexes[i].move_change.current)
					{
						hex->set_color(CHSV(Change::get_current_value(high_intensity_hexes->color_change), 255, 255));
					}
					else
					{
						// For current hex
						for (int j = 0; j < 2; j++)
						{
							float progress;
							int side_angle;
							if (j == 0)
							{
								progress = high_intensity_hexes[i].move_change.total / high_intensity_hexes[i].move_change.progress;
								side_angle = get_direction_angle(high_intensity_hexes[i].move_side);
							}
							else
							{
								progress = (float)1 - high_intensity_hexes[i].move_change.total / high_intensity_hexes[i].move_change.progress;
								side_angle = abs(FULL_ROTATION_ANGLE - get_direction_angle(high_intensity_hexes[i].move_side));
							}
							int hex_index = j == 0 ? high_intensity_hexes[i].move_change.current : high_intensity_hexes[i].move_change.next;

							hex = HexNS::hexes->get_by_index(i);
							for (int k = 0; k < hex->num_leds; k++)
							{
								int distance_to_center = get_distance_between_led_and_center(progress, side_angle, hex->get_angle_at_index(k));

								float scaled_proximity = distance_to_center > LOWEST_BRIGTHENESS_CENTER_DISTANCE ? 0 : 1 - (distance_to_center - PEAK_BRIGHTNESS_CENTER_DISTANCE) / (LOWEST_BRIGTHENESS_CENTER_DISTANCE - PEAK_BRIGHTNESS_CENTER_DISTANCE);
								CHSV background_color = CHSV(Change::get_current_value(background_colors[hex_index]), BACKGROUND_LIGHTNESS, BACKGROUND_SATURATION);
								CHSV foreground_color = CHSV(Change::get_current_value(high_intensity_hexes[i].color_change), 255, 255);
								hex->set_at_index(k, Util::fade_towards_color(background_color, foreground_color, scaled_proximity * 255));
							}
						}
					}
				}
			}
		}
	}

	namespace Effects
	{
		void set_led(String index, String color)
		{
			enable();
			animating = false;
			Leds::leds[atoi(index.c_str())] = parse_color(color);
			FastLED.show();
		}

		void set_led_in_hex(String index, String hex_id, String color)
		{
			enable();
			animating = false;
			HexNS::Hex *hex = HexNS::hexes->get_by_id(atoi(hex_id.c_str()));
			hex->set_at_index(atoi(index.c_str()), parse_color(color));
			FastLED.show();
		}

		void set_hex(String hex_id, String color)
		{
			enable();
			animating = false;
			HexNS::Hex *hex = HexNS::hexes->get_by_id(atoi(hex_id.c_str()));
			hex->set_color(parse_color(color));
			FastLED.show();
		}

		void enable_rainbow(int revolve_time)
		{
			enable();
			Animations::Rainbow::setup(revolve_time);
			animating = true;
			animation_fn = Animations::Rainbow::loop;
		}

		void enable_edge_rainbow(int revolve_time)
		{
			enable();
			Animations::EdgeRainbow::setup(revolve_time);
			animating = true;
			animation_fn = Animations::EdgeRainbow::loop;
		}

		void set_all(String str_color)
		{
			enable();
			animating = false;
			CRGB color = parse_color(str_color);
			for (int i = 0; i < HexNS::hexes->num_hexes; i++)
			{
				HexNS::hexes->get_by_index(i)->set_color(color);
			}
			FastLED.show();
		}

		void move_around()
		{
			enable();
			Animations::MoveAround::setup();
			animating = true;
			animation_fn = Animations::MoveAround::loop;
		}

		void random_colors_gradual(int wait_time_min, int wait_time_max, int neighbour_influence, bool use_pastel)
		{
			enable();
			Animations::RandomColorsGradual::setup(wait_time_min, wait_time_max, neighbour_influence, use_pastel);
			animating = true;
			animation_fn = Animations::RandomColorsGradual::loop;
		}

		void random_colors(int wait_time)
		{
			enable();
			Animations::RandomColors::setup(wait_time);
			animating = true;
			animation_fn = Animations::RandomColors::loop;
		}
	}

	bool enabled = true;
	bool enabling = false;
	bool disabling = false;
	int state_change_progress = 255;
	unsigned long state_change_start = millis();
	void enable()
	{
		if (enabled) return;
		enabled = true;
		enabling = true;
		state_change_progress = 0;
		state_change_start = millis();
	}

	void disable()
	{
		disabling = true;
		state_change_progress = 255;
		state_change_start = millis();
	}

	void loop()
	{
		if (animating && enabled)
		{
			animation_fn();

			if (enabling || disabling) {
				unsigned long time_passed = millis() - state_change_start;
				double percentage_transition_complete = Util::divide(time_passed, STATE_CHANGE_TIME);
				if (enabling)
				{
					state_change_progress = percentage_transition_complete * 255;
					if (time_passed >= STATE_CHANGE_TIME) {
						state_change_progress = 255;
						enabling = false;
					}
				}
				else if (disabling)
				{
					state_change_progress  = 255 - (percentage_transition_complete * 255);
					if (time_passed >= STATE_CHANGE_TIME)
					{
						state_change_progress = 0;
						disabling = false;
						enabled = false;
						FastLED.showColor(CRGB::Black);
						return;
					}
				}
			}

			if (state_change_progress != 255) {
				Leds::leds->nscale8(255);
			}
			FastLED.show(state_change_progress);
		}
	}
}
#include <hex.h>
#include <leds.h>
#include <string.h>
#include <FastLED.h>
#include <util.h>
#include <telnet.h>

#define MAX_CSHV_VALUE_MOD 256
#define MAX_CSHV_VALUE 255
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
#define STATE_CHANGE_TIME 1000
#define ENABLE_FADE_OUT 0

#define MIN_SPLIT_COLOR_DIFF 30

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
	bool (*animation_fn)();

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

	typedef struct split_color
	{
		HexNS::Hex *hex;
		change_t first_half_color;
		change_t second_half_color;
		int split_led_idx;
		bool uses_split;
	} split_color_t;

	namespace Change
	{
		change_t set_new_color_target(change_t prev_change, int interval_min, int interval_max)
		{
			change_t change;
			change.current = prev_change.next;
			change.progress = 0;
			change.total = random(interval_min, interval_max);
			change.next = random(0, MAX_CSHV_VALUE);
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
			CRGB current = CHSV(change.current, MAX_CSHV_VALUE, MAX_CSHV_VALUE);
			CRGB next = CHSV(change.next, MAX_CSHV_VALUE, MAX_CSHV_VALUE);
			double progress = Util::divide(change.progress, change.total);
			return Util::fade_towards_color(current, next, progress * MAX_CSHV_VALUE);
		}
	}

	namespace Split
	{
		split_color_t set_new_color_target(split_color_t prev_split, int interval_min, int interval_max, HexNS::Hex *hex, bool use_split, bool is_first = false)
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
				revolve_step = Util::divide(MAX_CSHV_VALUE, _revolve_time);
				last_iteration = 0;
				offset = 0;
			}

			bool loop()
			{
				long long time_diff = millis() - last_iteration;
				double added_offset = (double)time_diff * revolve_step;
				offset = std::fmod(offset + added_offset, MAX_CSHV_VALUE_MOD);
				last_iteration = millis();

				int rounded_offset = (int)round(offset);

				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					HexNS::Hex *hex = HexNS::hexes->get_by_index(i);
					int hex_led_step = (int)Util::divide(MAX_CSHV_VALUE_MOD, hex->num_leds);

					int total_step = rounded_offset;
					for (int j = 0; j < hex->num_leds; j++, total_step = (hex_led_step + total_step) % MAX_CSHV_VALUE_MOD)
					{
						hex->set_at_index(j, CHSV(total_step, MAX_CSHV_VALUE, MAX_CSHV_VALUE));
					}
				}

				return true;
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
				revolve_step = Util::divide(MAX_CSHV_VALUE, _revolve_time);
				last_iteration = 0;
				offset = 0;
			}

			bool loop()
			{
				long long time_diff = millis() - last_iteration;
				double added_offset = (double)time_diff * revolve_step;
				offset = std::fmod(offset + added_offset, MAX_CSHV_VALUE_MOD);
				last_iteration = millis();

				int rounded_offset = (int)round(offset);

				int total_offset = rounded_offset;
				for (int i = 0; i < NUM_LEDS; i++)
				{
					Leds::leds[i] = CHSV(total_offset, MAX_CSHV_VALUE, MAX_CSHV_VALUE);

					total_offset = (total_offset + RAINBOW_STEP) % MAX_CSHV_VALUE_MOD;
				}

				return true;
			}
		}

		namespace RandomColorsGradual
		{
			int wait_time_min = 0;
			int wait_time_max = 0;
			uint8_t neighbour_influence = 0;
			bool use_pastel = false;
			bool use_split = false;
			long long last_iteration;
			long long last_refresh_iteration;

			split_color_t colors[HEXES_UPPERBOUND];

			void setup(int _wait_time_min, int _wait_time_max, int _neighbour_influence, bool _use_pastel, bool _use_split)
			{
				wait_time_min = _wait_time_min;
				wait_time_max = _wait_time_max;
				neighbour_influence = _neighbour_influence;
				use_pastel = _use_pastel;
				use_split = _use_split;
				LOGF("Using params: wait_time_min %d, wait_time_max %d, neighbour_influence %d, use_pastel %d, use_split %d\n", wait_time_min, wait_time_max, neighbour_influence, use_pastel, use_split);
				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					split_color_t prev;
					prev.first_half_color.next = random(0, MAX_CSHV_VALUE);
					prev.second_half_color.next = random(0, MAX_CSHV_VALUE);

					HexNS::Hex *hex = HexNS::hexes->get_by_index(i);
					colors[i] = Split::set_new_color_target(prev, wait_time_min, wait_time_max, hex, use_split, true);
				}
				last_iteration = millis();
			}

			bool loop()
			{
				int time_diff = millis() - last_iteration;
				last_iteration = millis();

				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					Split::bump_progress(&colors[i], time_diff);
					colors[i] = Split::change_on_total_reached(colors[i], wait_time_min, wait_time_max);

					HexNS::Hex *hex = colors[i].hex;

					for (int j = 0; j < hex->num_leds; j++)
					{
						CRGB current_color = use_pastel ? Split::get_current_color(colors[i], j) : CHSV(Split::get_current_value(colors[i], j), MAX_CSHV_VALUE, MAX_CSHV_VALUE);
						HexNS::Hex *neighbour = hex->get_neighbour_at_led(j);

						if (neighbour == NULL)
						{
							hex->set_at_index(j, current_color);
						}
						else
						{
							// Get the color of the neighbour
							int neighbour_led_idx = (j + hex->num_leds) % hex->num_leds;
							CRGB neighbour_color = use_pastel ? Split::get_current_color(colors[neighbour->index], neighbour_led_idx) : CHSV(Split::get_current_value(colors[neighbour->index], neighbour_led_idx), MAX_CSHV_VALUE, MAX_CSHV_VALUE);

							// Move towards it a little
							hex->set_at_index(j, Util::fade_towards_color(current_color, neighbour_color, neighbour_influence));
						}
					}
				}
				return true;
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

			bool loop()
			{
				if (millis() - last_iteration <= wait_time)
				{
					return false;
				}

				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					HexNS::Hex *hex = HexNS::hexes->get_by_index(i);

					hex->set_color(CHSV(random(0, MAX_CSHV_VALUE), MAX_CSHV_VALUE, MAX_CSHV_VALUE));
				}

				last_iteration = millis();
				return true;
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

			bool loop()
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
						hex->set_color(CHSV(Change::get_current_value(high_intensity_hexes->color_change), MAX_CSHV_VALUE, MAX_CSHV_VALUE));
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
								CHSV foreground_color = CHSV(Change::get_current_value(high_intensity_hexes[i].color_change), MAX_CSHV_VALUE, MAX_CSHV_VALUE);
								hex->set_at_index(k, Util::fade_towards_color(background_color, foreground_color, scaled_proximity * MAX_CSHV_VALUE));
							}
						}
					}
				}

				return true;
			}
		}
	}

	namespace Effects
	{
		void set_led(int index, CRGB color)
		{
			enable();
			animating = false;
			animation_fn = NULL;
			Leds::leds[index] = color;
			FastLED.show();
		}

		void set_led_in_hex(int index, int hex_id, CRGB color)
		{
			enable();
			animating = false;
			animation_fn = NULL;
			HexNS::Hex *hex = HexNS::hexes->get_by_id(hex_id);
			hex->set_at_index(index, color);
			FastLED.show();
		}

		void set_hex(int hex_id, CRGB color)
		{
			enable();
			animating = false;
			animation_fn = NULL;
			HexNS::Hex *hex = HexNS::hexes->get_by_id(hex_id);
			hex->set_color(color);
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

		void set_all(CRGB str_color)
		{
			enable();
			animating = false;
			animation_fn = NULL;
			for (int i = 0; i < HexNS::hexes->num_hexes; i++)
			{
				HexNS::hexes->get_by_index(i)->set_color(str_color);
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

		void random_colors_gradual(int wait_time_min, int wait_time_max, int neighbour_influence, bool use_pastel, bool split)
		{
			enable();
			Animations::RandomColorsGradual::setup(wait_time_min, wait_time_max, neighbour_influence, use_pastel, split);
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
		if (animating && enabled && animation_fn)
		{
			if (!animation_fn())
			{
				return;
			}

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
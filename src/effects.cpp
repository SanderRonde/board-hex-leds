#include <hex.h>
#include <leds.h>
#include <string.h>
#include <FastLED.h>

#define MAX_CSHV_VALUE 256
#define NUM_HIGH_INTENSITY_HEXES 2
#define BACKGROUND_COLOR_CHANGE_MIN 10000
#define BACKGROUND_COLOR_CHANGE_MAX 20000
#define MOVE_CHANGE_MIN 5000
#define MOVE_CHANGE_MAX 8000
#define BACKGROUND_SATURATION 46
#define BACKGROUND_LIGHTNESS 73
#define MAX_MOVE_TARGET_ITERATIONS 1000
#define PEAK_BRIGHTNESS_CENTER_DISTANCE 1
#define LOWEST_BRIGTHENESS_CENTER_DISTANCE 2

namespace Effects
{
	CRGB parse_color(String color)
	{
		CRGB color_obj;
		char red[3];
		char green[3];
		char blue[3];

		const char *color_c_str = color.c_str();
		strncpy(red, color_c_str + 1, 2);
		strncpy(green, color_c_str + 3, 4);
		strncpy(blue, color_c_str + 5, 6);
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
				revolve_step = (double)255 / _revolve_time;
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
					int hex_led_step = MAX_CSHV_VALUE / hex->num_leds;

					int total_step = rounded_offset;
					for (int j = 0; j < hex->num_leds; j++, total_step = (hex_led_step + total_step) % MAX_CSHV_VALUE)
					{
						hex->set_at_index(j, CHSV(total_step, 255, 255));
					}
				}

				FastLED.show();
			}
		}

		namespace RandomColorsGradual
		{
			int wait_time = 0;
			long long last_iteration;
			int last_iteration_colors[HEXES_UPPERBOUND];
			int current_iteration_colors[HEXES_UPPERBOUND];

			void setup(int _wait_time)
			{
				wait_time = _wait_time;
				for (int i = 0; i < HEXES_UPPERBOUND; i++)
				{
					current_iteration_colors[i] = last_iteration_colors[i] = 0;
				}
			}

			void loop()
			{
				if (millis() - last_iteration > wait_time)
				{
					for (int i = 0; i < HexNS::hexes->num_hexes; i++)
					{
						last_iteration_colors[i] = current_iteration_colors[i];
						current_iteration_colors[i] = random(0, 255);
					}
				}

				int time_diff = millis() % wait_time;
				double time_diff_percentage = time_diff / wait_time;
				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					HexNS::Hex *hex = HexNS::hexes->get_by_index(i);
					int hue_diff = current_iteration_colors[i] - last_iteration_colors[i];
					hex->set_color(CHSV(last_iteration_colors[i] + (hue_diff * time_diff_percentage), 255, 255));
				}

				last_iteration = millis();
				FastLED.show();
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
				FastLED.show();
			}
		}

		namespace MoveAround
		{
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

			change_t set_new_color_target(change_t prev_change, int interval_min, int interval_max)
			{
				change_t change;
				change.current = prev_change.next;
				change.progress = 0;
				change.total = random(interval_min, interval_max);
				change.next = random(0, 255);
				return change;
			}

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
				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					background_colors[i] = set_new_color_target(prev, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
				}

				for (int i = 0; i < NUM_HIGH_INTENSITY_HEXES; i++)
				{
					high_intensity_hex_t hex;
					hex.index = random(0, HexNS::hexes->num_hexes);
					hex.color_change = set_new_color_target(prev, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
					hex.move_side = (HexNS::hex_side_t)random(0, HEX_SIDES);
					change_t move_change;
					move_change.current = hex.index;
					move_change.next = random_except(0, HexNS::hexes->num_hexes, hex.index);
					move_change.progress = 0;
					move_change.total = random(MOVE_CHANGE_MIN, MOVE_CHANGE_MAX);
					hex.move_change = move_change;
				}

				last_iteration = millis();
			}

			int get_current_value(change_t change)
			{
				int diff = change.next - change.current;
				double progress = change.progress / change.total;
				return change.current + (diff * progress);
			}

			int get_direction_angle(HexNS::hex_side_t direction)
			{
				return (direction * ANGLE_PER_SIDE) + (ANGLE_PER_SIDE / 2);
			}

			int mod_positive(int base, int modulo)
			{
				return base - floor(base / modulo) * modulo;
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

			uint8_t nblendU8TowardU8(uint8_t cur, const uint8_t target, uint8_t amount)
			{
				if (cur == target)
					return;

				if (cur < target)
				{
					uint8_t delta = target - cur;
					delta = scale8_video(delta, amount);
					return cur + delta;
				}
				else
				{
					uint8_t delta = cur - target;
					delta = scale8_video(delta, amount);
					return cur - delta;
				}
			}

			CRGB fadeTowardColor(CRGB cur, const CRGB target, uint8_t amount)
			{
				CRGB blended;
				blended.red = nblendU8TowardU8(cur.red, target.red, amount);
				blended.green = nblendU8TowardU8(cur.green, target.green, amount);
				blended.blue = nblendU8TowardU8(cur.blue, target.blue, amount);
				return blended;
			}

			void loop()
			{
				long long time_diff = millis() - last_iteration;

				// Paint backgrounds
				for (int i = 0; i < HexNS::hexes->num_hexes; i++)
				{
					HexNS::Hex *hex = HexNS::hexes->get_by_index(i);
					background_colors[i].progress += time_diff;
					if (background_colors[i].progress > background_colors[i].total)
					{
						// Choose a new color target
						background_colors[i] = set_new_color_target(background_colors[i], BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
					}

					// Find color
					int current_hue = get_current_value(background_colors[i]);
					hex->set_color(CHSV(current_hue, BACKGROUND_SATURATION, BACKGROUND_LIGHTNESS));
				}

				for (int i = 0; i < NUM_HIGH_INTENSITY_HEXES; i++)
				{
					HexNS::Hex *hex = HexNS::hexes->get_by_index(i);

					// Do color change
					high_intensity_hexes[i].color_change.progress += time_diff;
					if (high_intensity_hexes[i].color_change.progress > high_intensity_hexes[i].color_change.total)
					{
						// Choose a new color target
						high_intensity_hexes[i].color_change = set_new_color_target(high_intensity_hexes[i].color_change, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
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
						hex->set_color(CHSV(get_current_value(high_intensity_hexes->color_change), 255, 255));
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
								CHSV background_color = CHSV(get_current_value(background_colors[hex_index]), BACKGROUND_LIGHTNESS, BACKGROUND_SATURATION);
								CHSV foreground_color = CHSV(get_current_value(high_intensity_hexes[i].color_change), 255, 255);
								hex->set_at_index(k, fadeTowardColor(background_color, foreground_color, scaled_proximity * 255));
							}
						}
					}
				}

				FastLED.show();
			}
		}
	}

	namespace Effects
	{
		void set_led(String index, String color)
		{
			Leds::leds[atoi(index.c_str())] = parse_color(color);
			FastLED.show();
		}

		void set_led_in_hex(String index, String hex_id, String color)
		{
			HexNS::Hex *hex = HexNS::hexes->get_by_id(atoi(hex_id.c_str()));
			hex->set_at_index(atoi(index.c_str()), parse_color(color));
			FastLED.show();
		}

		void set_hex(String hex_id, String color)
		{
			HexNS::Hex *hex = HexNS::hexes->get_by_id(atoi(hex_id.c_str()));
			hex->set_color(parse_color(color));
			FastLED.show();
		}

		void enable_rainbow(String revolve_time)
		{
			Animations::Rainbow::setup(atoi(revolve_time.c_str()));
			animating = true;
			animation_fn = Animations::Rainbow::loop;
		}

		void set_all(String str_color)
		{
			CRGB color = parse_color(str_color);
			for (int i = 0; i < HexNS::hexes->num_hexes; i++)
			{
				HexNS::hexes->get_by_index(i)->set_color(color);
			}
			FastLED.show();
		}

		void move_around()
		{
			Animations::MoveAround::setup();
			animating = true;
			animation_fn = Animations::MoveAround::loop;
		}

		void random_colors_gradual(String wait_time)
		{
			Animations::RandomColorsGradual::setup(atoi(wait_time.c_str()));
			animating = true;
			animation_fn = Animations::RandomColorsGradual::loop;
		}

		void random_colors(String wait_time)
		{
			Animations::RandomColors::setup(atoi(wait_time.c_str()));
			animating = true;
			animation_fn = Animations::RandomColors::loop;
		}
	}

	void loop()
	{
		if (animating)
		{
			animation_fn();
		}
	}
}
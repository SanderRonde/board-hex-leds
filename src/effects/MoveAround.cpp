#include "effects/MoveAround.h"
#include "effects/EffectBase.h"
#include "effects/change.h"
#include "constants.h"
#include "effects/change.h"

int MoveAround::random_except(int min, int max, int except)
{
	int random_num = random(min, max);
	while (random_num == except)
	{
		random_num = random(min, max);
	}
	return random_num;
}

int MoveAround::choose_new_move_direction(Hexes *hexes, int current_target)
{
	int iterations = 0;

	hex_side_t random_direction;
	while (true)
	{
		iterations++;

		if (iterations > MAX_MOVE_TARGET_ITERATIONS)
		{
			return -1;
		}
		random_direction = (hex_side_t)random(0, HEX_SIDES);

		Hex *target = hexes->get_by_id(current_target)->get_neighbour(random_direction);

		// Check if the neighbour exists
		if (target == NULL)
			continue;
		// Check if they are not currently being animated towards or from
		for (int i = 0; i < NUM_HIGH_INTENSITY_HEXES; i++)
		{
			// Ignore the one we're currently choosing for
			if (_high_intensity_hexes[i].move_change.next != current_target)
			{
				if (target->id == _high_intensity_hexes[i].move_change.next ||
						target->id == _high_intensity_hexes[i].move_change.current)
				{
					continue;
				}
			}
		}

		return random_direction;
	}
}

change_t MoveAround::set_new_move_target(Hexes *hexes, int current_id)
{
	change_t move_change;
	move_change.current = current_id;
	int move_direction = choose_new_move_direction(hexes, current_id);
	if (move_direction == -1)
	{
		move_change.next = current_id;
	}
	else
	{
		move_change.next = hexes->get_by_id(current_id)->get_neighbour((hex_side_t)move_direction)->id;
	}

	move_change.progress = 0;
	move_change.total = random(MOVE_CHANGE_MIN, MOVE_CHANGE_MAX);
	return move_change;
}

int MoveAround::get_direction_angle(hex_side_t direction)
{
	return (direction * ANGLE_PER_SIDE) + (ANGLE_PER_SIDE / 2);
}

int MoveAround::mod_positive(int base, int modulo)
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
int MoveAround::get_distance_between_led_and_center(float center_progress, int center_angle, int led_angle)
{
	int AB = center_progress * 2;
	int AC = 1;
	int A_angle = abs(mod_positive(center_angle - led_angle + 180, 360) - 180);

	int BC_squared = sq(AB) + sq(AC) - (2 * AB * AC * cos8(A_angle));
	return sqrt16(BC_squared);
}

MoveAround::MoveAround(Hexes *hexes) : EffectBase()
{
	change_t prev;
	prev.next = 0;
#ifdef MOVE_SAME_BACKGROUND
	_background_colors[0] = Change::set_new_color_target(prev, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
#else
	for (int i = 0; i < hexes->num_hexes; i++)
	{
		_background_colors[i] = set_new_color_target(prev, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
	}
#endif

	for (int i = 0; i < NUM_HIGH_INTENSITY_HEXES; i++)
	{
		high_intensity_hex *hex = &_high_intensity_hexes[i];

		bool hex_taken = false;
		do
		{
			hex_taken = false;
			hex->index = random(0, hexes->num_hexes);
			for (int j = 0; j < i; j++)
			{
				if (_high_intensity_hexes[j].index == hex->index)
				{
					hex_taken = true;
					break;
				}
			}
		} while (hex_taken);
		hex->color_change = Change::set_new_color_target(prev, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
		hex->move_side = (hex_side_t)random(0, HEX_SIDES);
		change_t move_change;
		move_change.current = hex->index;
		move_change.next = random_except(0, hexes->num_hexes, hex->index);
		move_change.progress = 0;
		move_change.total = random(MOVE_CHANGE_MIN, MOVE_CHANGE_MAX);
		hex->move_change = move_change;
	}

	_last_iteration = millis();
}

bool MoveAround::loop(Hexes *hexes)
{
	long long time_diff = millis() - _last_iteration;
	_last_iteration = millis();

	// Paint backgrounds
#ifndef MOVE_SAME_BACKGROUND
	for (int hex_idx = 0; hex_idx < hexes->num_hexes; hex_idx++)
	{
#endif
#ifdef MOVE_SAME_BACKGROUND
		int hex_idx = 0;
#endif
		Hex *hex = hexes->get_by_index(hex_idx);
		_background_colors[hex_idx].progress += time_diff;
		if (_background_colors[hex_idx].progress > _background_colors[hex_idx].total)
		{
			// Choose a new color target
			_background_colors[hex_idx] = Change::set_new_color_target(_background_colors[hex_idx], BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
		}

		// Find color
		int current_hue = Change::get_current_value(_background_colors[hex_idx]);
#ifdef MOVE_SAME_BACKGROUND
		for (size_t i = 0; i < hexes->num_hexes; i++)
		{
			hex = hexes->get_by_index(i);
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
		Hex *hex = hexes->get_by_index(i);

		// Do color change
		_high_intensity_hexes[i].color_change.progress += time_diff;
		if (_high_intensity_hexes[i].color_change.progress > _high_intensity_hexes[i].color_change.total)
		{
			// Choose a new color target
			_high_intensity_hexes[i].color_change = Change::set_new_color_target(_high_intensity_hexes[i].color_change, BACKGROUND_COLOR_CHANGE_MIN, BACKGROUND_COLOR_CHANGE_MAX);
		}

		// Do move change
		_high_intensity_hexes[i].move_change.progress += time_diff;
		if (_high_intensity_hexes[i].move_change.progress > _high_intensity_hexes[i].move_change.total)
		{
			// Choose a new move target
			_high_intensity_hexes[i].move_change = set_new_move_target(hexes, _high_intensity_hexes[i].move_change.next);
		}

		// Do the coloring
		if (_high_intensity_hexes[i].move_change.next == _high_intensity_hexes[i].move_change.current)
		{
			hex->set_color(CHSV(Change::get_current_value(_high_intensity_hexes->color_change), MAX_CSHV_VALUE, MAX_CSHV_VALUE));
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
					progress = _high_intensity_hexes[i].move_change.total / _high_intensity_hexes[i].move_change.progress;
					side_angle = get_direction_angle(_high_intensity_hexes[i].move_side);
				}
				else
				{
					progress = (float)1 - _high_intensity_hexes[i].move_change.total / _high_intensity_hexes[i].move_change.progress;
					side_angle = abs(FULL_ROTATION_ANGLE - get_direction_angle(_high_intensity_hexes[i].move_side));
				}
				int hex_index = j == 0 ? _high_intensity_hexes[i].move_change.current : _high_intensity_hexes[i].move_change.next;

				hex = hexes->get_by_index(i);
				for (size_t k = 0; k < hex->num_leds; k++)
				{
					int distance_to_center = get_distance_between_led_and_center(progress, side_angle, hex->get_angle_at_index(k));

					float scaled_proximity = distance_to_center > LOWEST_BRIGTHENESS_CENTER_DISTANCE ? 0 : 1 - (distance_to_center - PEAK_BRIGHTNESS_CENTER_DISTANCE) / (LOWEST_BRIGTHENESS_CENTER_DISTANCE - PEAK_BRIGHTNESS_CENTER_DISTANCE);
					CHSV background_color = CHSV(Change::get_current_value(_background_colors[hex_index]), BACKGROUND_LIGHTNESS, BACKGROUND_SATURATION);
					CHSV foreground_color = CHSV(Change::get_current_value(_high_intensity_hexes[i].color_change), MAX_CSHV_VALUE, MAX_CSHV_VALUE);
					hex->set_at_index(k, Util::fade_towards_color(background_color, foreground_color, scaled_proximity * MAX_CSHV_VALUE));
				}
			}
		}
	}
	return true;
}
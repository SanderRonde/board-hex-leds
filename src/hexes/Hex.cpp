#include <hex.h>
#include <util.h>
#include <FastLED.h>
#include "leds.h"

template <class T>
void reverse_arr(T *arr, int arr_len)
{
	for (int i = 0; i < floor((double)arr_len / 2); i++)
	{
		T temp = arr[i];
		int end_index = arr_len - (i + 1);
		arr[i] = arr[end_index];
		arr[end_index] = temp;
	}
}

template <class T>
void shift_array_once(T *arr, int arr_len)
{
	T last_item = arr[arr_len - 1];
	for (int i = 0; i < arr_len; i++)
	{
		T temp = arr[i];
		arr[i] = last_item;
		last_item = temp;
	}
}

template <class T>
void shift_array(T *arr, int arr_len, int shift_amount)
{
	if (shift_amount < 0)
	{
		shift_amount += arr_len;
	}
	for (int i = 0; i < shift_amount; i++)
	{
		shift_array_once<T>(arr, arr_len);
	}
}

Hex::Hex(hex_describer_t described_hex)
{
	id = described_hex.id;

	num_leds = 0;
	for (int i = 0; described_hex.leds[i] != -1; i++, num_leds++)
	{
	}

	_led_indices = (int *)malloc(sizeof(int) * num_leds);
	memcpy(_led_indices, described_hex.leds, sizeof(int) * num_leds);
	if (described_hex.invert_order)
	{
		reverse_arr<int>(_led_indices, num_leds);
	}
	if (described_hex.offset_percentage != 0)
	{
		int offset_flat_amount = round(num_leds * (double)described_hex.offset_percentage / 100);
		shift_array<int>(_led_indices, num_leds, offset_flat_amount);
	}
	memcpy(_sides, described_hex.borders, sizeof(int) * HEX_SIDES);
}
Hex::~Hex()
{
	free(_led_indices);
}

int Hex::operator[](int index)
{
	return get_at_index(index);
};

int Hex::get_at_index(int index)
{
	return _led_indices[index];
};

void Hex::set_at_index(int index, CRGB color)
{
	Leds::leds[get_at_index(index)] = color;
};

void Hex::set_color(CRGB color)
{
	for (size_t i = 0; i < num_leds; i++)
	{
		Leds::leds[_led_indices[i]] = color;
	}
};

Hex *Hex::get_neighbour(hex_side_t side)
{
	int side_id = _sides[side];
	if (side_id == -1)
	{
		return NULL;
	}
	return ((Hexes *)parent)->get_by_id(side_id);
}

hex_side_t Hex::get_side_at_index(int led_index)
{
	int side_index_unfixed = (int)floor((Util::divide(led_index, num_leds) * HEX_SIDES) - 0.5);
	return (hex_side_t)((HEX_SIDES + side_index_unfixed) % HEX_SIDES);
}

Hex *Hex::get_neighbour_hex_at_led(int led_index)
{
	return get_neighbour(get_side_at_index(led_index));
}

int Hex::get_neighbour_led_at_led(int led_index)
{
	auto neighbour = get_neighbour_hex_at_led(led_index);
	if (neighbour == NULL)
	{
		return -1;
	}

	// Get LED at angle ANGLE-180
	int self_led_angle = get_angle_at_index(led_index);
	int neighbour_led_angle = (self_led_angle + 180) % 360;

	return neighbour->get_led_at_angle(neighbour_led_angle);
}

/**
 * Calculate the step size in order to reach a full
 * revolution in given `revolution_time`
 */
float Hex::get_step_size_for_revolution(long revolution_time)
{
	return revolution_time / num_leds;
}

int Hex::get_led_at_angle(int angle)
{
	int smallest_diff = 0;
	int smallest_diff_index = 0;

	for (size_t i = 0; i < num_leds; i++)
	{
		int led_angle = get_angle_at_index(i);
		int diff = abs(led_angle - angle);
		if (diff < smallest_diff)
		{
			smallest_diff = diff;
			smallest_diff_index = i;
		}
	}

	return smallest_diff_index;
}

int Hex::get_angle_at_index(int index)
{
	int angle_per_led = Util::divide(FULL_ROTATION_ANGLE, num_leds);
	return ((index * angle_per_led) + (ANGLE_PER_SIDE / 2)) % FULL_ROTATION_ANGLE;
}

int Hex::get_angle_aligned_with_x_for_index(int index)
{
	int angle = get_angle_at_index(index);
	return (angle + (FULL_ROTATION_ANGLE / 4)) % FULL_ROTATION_ANGLE;
}

/**
 * Left is 0, right is 1
 */
float Hex::get_relative_x_pos_for_index(int index)
{
	int angle = get_angle_aligned_with_x_for_index(index);
	return ((cos(radians(angle))) + 1) / 2;
}

/**
 * Top is 0, bottom is 1
 */
float Hex::get_relative_y_pos_for_index(int index)
{
	int angle = get_angle_aligned_with_x_for_index(index);
	return ((sin(radians(angle))) + 1) / 2;
}

/**
 * Left is 0, right is 1
 * Top is 0, bottom is 1
 */
float Hex::get_relative_pos_for_index(int index, bool is_x)
{
	return is_x ? get_relative_x_pos_for_index(index) : get_relative_y_pos_for_index(index);
}

float Hex::get_led_global_x_position_at_index(int led_index)
{
	return ((Hexes *)parent)->get_led_x_pos_for_index(get_at_index(led_index));
}

float Hex::get_led_global_y_position_at_index(int led_index)
{
	return ((Hexes *)parent)->get_led_y_pos_for_index(get_at_index(led_index));
}
#include <hex.h>
#include <leds.h>

HexNS::Hex::Hex(hex_describer_t described_hex)
{
	id = described_hex.id;

	num_leds = 0;
	for (int i = 0; described_hex.leds[i] != -1; i++, num_leds++)
	{
	}

	_led_indices = (int *)malloc(sizeof(int) * num_leds);
	memcpy(_led_indices, described_hex.leds, sizeof(int) * num_leds);
	memcpy(_sides, described_hex.borders, sizeof(int) * HEX_SIDES);
}
HexNS::Hex::~Hex()
{
	free(_led_indices);
}

int HexNS::Hex::operator[](int index)
{
	return get_at_index(index);
};

int HexNS::Hex::get_at_index(int index)
{
	return _led_indices[index];
};

void HexNS::Hex::set_at_index(int index, CRGB color)
{
	Leds::leds[get_at_index(index)] = color;
};

void HexNS::Hex::set_color(CRGB color)
{
	for (int i = 0; i < num_leds; i++)
	{
		Leds::leds[_led_indices[i]] = color;
	}
};

HexNS::Hex *HexNS::Hex::get_neighbour(HexNS::hex_side_t side)
{
	return ((HexNS::Hexes *)parent)->get_by_id(side);
}

/**
 * Calculate the step size in order to reach a full
 * revolution in given `revolution_time`
 */
float HexNS::Hex::get_step_size_for_revolution(long revolution_time)
{
	return revolution_time / num_leds;
}

int HexNS::Hex::get_angle_at_index(int index)
{
	int angle_per_led = FULL_ROTATION_ANGLE / num_leds;
	return ((index * angle_per_led) + (ANGLE_PER_SIDE / 2)) % FULL_ROTATION_ANGLE;
}
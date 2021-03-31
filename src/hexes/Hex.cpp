#include <hex.h>
#include <leds.h>

HexNS::Hex::Hex(int _id, int _num_leds, int *led_indices, int sides[HEX_SIDES])
{
	id = _id;
	num_leds = _num_leds;
	_led_indices = (int *)malloc(sizeof(int) * num_leds);
	memcpy(_led_indices, led_indices, sizeof(int) * num_leds);
	memcpy(_sides, sides, sizeof(int) * HEX_SIDES);
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

HexNS::Hex *HexNS::Hex::from_json(JsonObject hex_json)
{
	int id = hex_json["id"];
	JsonArray borders_json = hex_json["borders"];
	JsonArray leds_json = hex_json["leds"];
	size_t num_leds = leds_json.size();

	int *borders = (int *)malloc(sizeof(int) * borders_json.size());
	for (int i = 0; i < borders_json.size(); i++)
	{
		borders[i] = borders_json[i];
	}
	int *leds = (int *)malloc(sizeof(int) * leds_json.size());
	for (int i = 0; i < leds_json.size(); i++)
	{
		leds[i] = leds_json[i];
	}

	HexNS::Hex hex = HexNS::Hex(id, num_leds, leds, borders);
	return &hex;
}

HexNS::Hex *HexNS::Hex::get_neighbour(HexNS::hex_side_t side)
{
	return parent->get_by_id(side);
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
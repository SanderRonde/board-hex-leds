#include <hex.h>
#include "leds.h"

using namespace std;

Hex *Hexes::get_by_id(int id)
{
	for (int i = 0; i < num_hexes; i++)
	{
		if ((*_hexes[i]).id == id)
		{
			return _hexes[i];
		}
	}
	return NULL;
}

Hex *Hexes::get_by_index(int index)
{
	return _hexes[index];
}

void Hexes::set_led_at_index(int index, CRGB color)
{
	Leds::leds[index] = color;
}

int Hexes::get_x_max_pos()
{
	int max_value = 0;
	for (const auto &pair : _positions->hex_positions_x)
	{
		max_value = max(max_value, pair.second);
	}
	return max_value;
}

int Hexes::get_y_max_pos()
{
	int max_value = 0;
	for (const auto &pair : _positions->hex_positions_y)
	{
		max_value = max(max_value, pair.second);
	}
	return max_value;
}

int Hexes::get_hex_x_pos_for_index(int index)
{
	return _positions->hex_positions_x[index];
}

int Hexes::get_hex_y_pos_for_index(int index)
{
	return _positions->hex_positions_y[index];
}

float Hexes::get_led_x_pos_for_index(int led_index)
{
	return _positions->led_pos_map_x[led_index];
}
float Hexes::get_led_y_pos_for_index(int led_index)
{
	return _positions->led_pos_map_y[led_index];
}

Hexes::Hexes(Hex **passed_hexes, unsigned int passed_num_hexes)
{
	_hexes = passed_hexes;
	num_hexes = passed_num_hexes;
};

Hexes::~Hexes()
{
	free(_hexes);
	free(_positions);
}

Hexes *Hexes::from_described(const hex_describer_t hexes[], size_t num_hexes)
{
	Hex **created_hexes = (Hex **)malloc(sizeof(Hex *) * num_hexes);
	for (size_t i = 0; i < num_hexes; i++)
	{
		created_hexes[i] = new Hex(hexes[i]);
	}

	Hexes *hexes_cls = new Hexes(created_hexes, num_hexes);
	for (size_t i = 0; i < num_hexes; i++)
	{
		(*created_hexes[i]).parent = hexes_cls;
		(*created_hexes[i]).index = i;
	}

	hexes_cls->_positions = new HexPositions(hexes_cls);

	return hexes_cls;
}
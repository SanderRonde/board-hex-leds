#pragma once

#include <FastLED.h>
#include <map>

#define HEX_SIDES 6
#define HEXES_UPPERBOUND 20
#define MAX_JSON_DOC_SIZE 2048
#define FULL_ROTATION_ANGLE 360
#define ANGLE_PER_SIDE (FULL_ROTATION_ANGLE / HEX_SIDES)
#define DECLARE_DESCRIBER(name) \
	extern const hex_describer_t described_hexes_##name[]; \
	extern size_t num_hexes_##name;
#define DEFINE_DESCRIBER(name) \
	const hex_describer_t described_hexes_##name[]
#define DEFINE_DESCRIBER_SIZE(name) \
	size_t num_hexes_##name = sizeof(described_hexes_##name) / sizeof(hex_describer_t);
#define _USE_DESCRIBER(name) \
	described_hexes_##name
#define _USE_DESCRIBER_NUM(name) \
	num_hexes_##name
#define USE_DESCRIBER(name) _USE_DESCRIBER(name)
#define USE_DESCRIBER_NUM(name) _USE_DESCRIBER_NUM(name)

typedef struct hex_describer
{
	int id;
	/**
	 * When this is a positive number, leds are moved clockwise
	 */
	int offset_percentage;
	bool invert_order;
	int borders[HEX_SIDES];
	int leds[35];
} hex_describer_t;

typedef enum HEX_SIDE
{
	HEX_SIDE_TOP = 0,
	HEX_SIDE_TOP_RIGHT = 1,
	HEX_SIDE_BOT_RIGHT = 2,
	HEX_SIDE_BOT = 3,
	HEX_SIDE_BOT_LEFT = 4,
	HEX_SIDE_TOP_LEFT = 5,
} hex_side_t;

class HexPositions
{
public:
	// A map from hex index to their X position (LTR)
	std::map<int, int> hex_positions_x;
	// A map from hex index to their Y position (BTT)
	std::map<int, int> hex_positions_y;
	// A map from hex index to their X position (LTR)
	std::map<int, float> hex_positions_x_relative;
	// A map from hex index to their Y position (BTT)
	std::map<int, float> hex_positions_y_relative;
	// A map from led index to their X position (LTR)

	HexPositions(void *hexes);
	// A map from led index to their X position (LTR)
	std::map<int, float> get_led_pos_map_x();
	// A map from led index to their Y position (BTT)
	std::map<int, float> get_led_pos_map_y();

private:
	void *_hexes;
	bool _pos_maps_set = false;
	void ensure_pos_maps();
	// A map from led index to their X position (LTR)
	std::map<int, float> _led_pos_map_x;
	// A map from led index to their Y position (BTT)
	std::map<int, float> _led_pos_map_y;
};

class Hex
{
public:
	void *parent;
	int id;
	int index;
	size_t num_leds;

	Hex(hex_describer_t described_hex);
	~Hex();

	int operator[](int index);
	int get_at_index(int index);
	void set_at_index(int index, CRGB color);
	void set_color(CRGB color);
	int get_led_at_angle(int angle);
	int get_angle_at_index(int index);
	int get_angle_aligned_with_x_for_index(int index);
	float get_relative_x_pos_for_index(int index);
	float get_relative_y_pos_for_index(int index);
	float get_relative_pos_for_index(int index, bool is_y);
	float get_led_global_x_position_at_index(int led_index);
	float get_led_global_y_position_at_index(int led_index);

	hex_side_t get_side_at_index(int led_index);
	Hex *get_neighbour(hex_side_t side);
	Hex *get_neighbour_hex_at_led(int led_index);
	int get_neighbour_led_at_led(int led_index);

	/**
	 * Calculate the step size in order to reach a full
	 * revolution in given `revolution_time`
	 */
	float get_step_size_for_revolution(long revolution_time);

private:
	int _sides[HEX_SIDES];
	int *_led_indices;
};

class Hexes
{
public:
	Hexes(Hex **passed_hexes, unsigned int num_hexes);
	~Hexes();
	Hex *get_by_id(int id);
	Hex *get_by_index(int index);
	void set_led_at_index(int index, CRGB color);
	int get_x_max_pos();
	int get_y_max_pos();
	int get_hex_x_pos_for_index(int index);
	int get_hex_y_pos_for_index(int index);
	float get_led_x_pos_for_index(int led_index);
	float get_led_y_pos_for_index(int led_index);
	Hexes *hexes = NULL;
	size_t num_hexes;

	static Hexes *from_described(const hex_describer_t hexes[], size_t num_hexes);

private:
	Hex **_hexes;
	HexPositions *_positions;
};

DECLARE_DESCRIBER(bed)
DECLARE_DESCRIBER(desk)

namespace HexNS
{
	Hexes *setup();
};

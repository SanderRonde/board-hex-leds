#pragma once

#include <ArduinoJson.h>
#include <FastLED.h>

#define HEX_SIDES 6
#define HEXES_UPPERBOUND 20
#define MAX_JSON_DOC_SIZE 2048
#define FULL_ROTATION_ANGLE 360
#define ANGLE_PER_SIDE (FULL_ROTATION_ANGLE / HEX_SIDES)

namespace HexNS
{
	typedef enum HEX_SIDE
	{
		HEX_SIDE_TOP = 0,
		HEX_SIDE_TOP_RIGHT = 1,
		HEX_SIDE_BOT_RIGHT = 2,
		HEX_SIDE_BOT = 3,
		HEX_SIDE_BOT_LEFT = 4,
		HEX_SIDE_TOP_LEFT = 5,
	} hex_side_t;

	class Hex
	{
	public:
		Hexes *parent;
		int id;
		int num_leds;

		Hex(int _id, int _num_leds, int *led_indices, int sides[HEX_SIDES]);
		~Hex();

		int operator[](int index);
		int get_at_index(int index);
		void set_at_index(int index, CRGB color);
		void set_color(CRGB color);
		int get_angle_at_index(int index);

		static Hex *from_json(JsonObject hex_json);

		Hex *get_neighbour(hex_side_t side);

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
		Hexes *hexes = NULL;
		int num_hexes;

		static Hexes *Hexes::from_json(String json);

	private:
		Hex **_hexes;
	};

	void setup();

	Hexes *hexes;
};
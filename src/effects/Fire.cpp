#include "config.h"
#include "effects.h"
#include "constants.h"
#include "hex.h"
#include "telnet.h"
#include "util.h"
#include "api.h"
#include "effects/Fire.h"
#include "effects/EffectBase.h"
#include "color-util.h"

const CRGB FIRE_RED = CRGB(255, 0, 0);
const int SPARK_ITERATION_TIME = 1;
const int SPARK_COOL_MIN = 10;
const int SPARK_COOL_MAX = 15;
const byte SPARK_DROP_FACTOR_TOP = 3;
const byte SPARK_DROP_FACTOR_BOT = 5;
const int MIN_HEAT = 5;
const int MAX_HEAT = 15;
const int SPARK_CHANCE = 5;

std::vector<std::vector<Hex *>> get_columns(Hexes *hexes)
{
	std::map<int, std::map<int, Hex *>> unsorted_columns;
	for (size_t i = 0; i < hexes->num_hexes; i++)
	{
		int x_offset = hexes->get_hex_x_pos_for_index(i);
		int y_offset = hexes->get_hex_y_pos_for_index(i);
		if (unsorted_columns.count(x_offset) == 0)
		{
			std::map<int, Hex *> y_map;
			unsorted_columns[x_offset] = y_map;
		}

		unsorted_columns[x_offset][y_offset] = hexes->get_by_index(i);
	}

	/// Sort keys
	std::vector<int> x_keys;
	for (auto it = unsorted_columns.begin(); it != unsorted_columns.end(); ++it)
	{
		x_keys.push_back(it->first);
	}
	std::sort(x_keys.begin(), x_keys.end());

	std::vector<std::vector<Hex *>> columns;
	for (size_t i = 0; i < x_keys.size(); i++)
	{
		// Collect Y keys
		auto unsorted_column = unsorted_columns.at(x_keys.at(i));
		std::vector<int> y_keys;
		for (auto it = unsorted_column.begin(); it != unsorted_column.end(); ++it)
		{
			y_keys.push_back(it->first);
		}
		std::sort(y_keys.begin(), y_keys.end());

		std::vector<Hex *> column;
		for (size_t j = 0; j < y_keys.size(); j++)
		{
			column.push_back(unsorted_column.at(y_keys.at(j)));
		}
		columns.push_back(column);
	}

	return columns;
}

Spark::Spark(std::vector<Hex *> *column)
{
	this->column = column;
	this->progress = 0;
	this->heat = random(MIN_HEAT, MAX_HEAT);
}

bool Spark::loop(Hexes *hexes)
{
	this->progress = min((long)this->progress + random(SPARK_COOL_MIN, SPARK_COOL_MAX), 255L);
	if (this->progress == 255)
	{
		return false;
	}

	return true;
}

// TODO: should require RequestObj and get its params from that
Fire::Fire(Hexes *hexes) : EffectBase()
{
	yield();
	_columns = get_columns(hexes);
	yield();

	for (size_t i = 0; i < _columns.size(); i++)
	{
		auto column = _columns.at(i);
		_sparks_by_column[i] = new std::vector<Spark *>();
	}
}

/**
 * The idea: have the bottom-most row be superhot (yellow/white)
 * and occasionally have a "spark" trickle up. It just follows
 * the LEDs up.
 * Maybe: sparks are entire panels at once.
 * Maybe: have it make a choice at every split on what direction to go in
 */

bool Fire::loop(Hexes *hexes)
{
	// Bottom layer fades, the rest is red
	for (size_t i = 0; i < hexes->num_hexes; i++)
	{
		hexes->get_by_index(i)->set_color(FIRE_RED);
	}

	bool is_spark_iteration = millis() - _last_spark_iteration > SPARK_ITERATION_TIME;
	if (is_spark_iteration)
	{
		_last_spark_iteration = millis();
	}
	if (is_spark_iteration)
	{
		// Randomly generate sparks
		for (size_t i = 0; i < _columns.size(); i++)
		{
			if (random(0, 100) < SPARK_CHANCE)
			{
				Spark *spark = new Spark(&_columns.at(i));
				_sparks_by_column.at(i)->push_back(spark);
			}
		}
	}

	// Draw sparks
	for (size_t i = 0; i < _sparks_by_column.size(); i++)
	{
		auto sparks_in_column = _sparks_by_column.at(i);
		auto column = _columns.at(i);

		byte spark_map[256] = {0};
		byte heat_map[256] = {0};
		for (size_t j = 0; j < sparks_in_column->size(); j++)
		{
			auto spark = sparks_in_column->at(j);
			int current_index = spark->progress;
			byte current_heat = 255;
			while (current_heat > 0 && current_index <= 255)
			{
				if (current_heat > spark_map[current_index])
				{
					spark_map[current_index] = current_heat;
					heat_map[current_index] = spark->heat;
				}
				current_index++;
				current_heat = max(0, (int)current_heat - SPARK_DROP_FACTOR_TOP);
			}
			current_index = spark->progress;
			current_heat = 255;
			while (current_heat > 0 && current_index >= 0)
			{
				if (current_heat > spark_map[current_index])
				{
					spark_map[current_index] = current_heat;
					heat_map[current_index] = spark->heat;
				}
				current_index--;
				current_heat = max(0, (int)current_heat - SPARK_DROP_FACTOR_BOT);
			}
		}

		// Apply map to all pixels in this column
		for (size_t j = 0; j < column.size(); j++)
		{
			auto hex = column.at(j);
			for (size_t k = 0; k < hex->num_leds; k++)
			{
				int bin = (int)floor(hex->get_led_global_y_position_at_index(k) * (float)255);
				double progress = Util::divide(spark_map[bin], 255);
				hex->set_at_index(k, fade_towards_color(CRGB(255, 0, 0), CRGB(255, heat_map[bin], 0), progress));
			}
		}
	}

	if (is_spark_iteration)
	{
		// Move sparks forward
		for (size_t i = 0; i < _sparks_by_column.size(); i++)
		{
			auto sparks_in_column = _sparks_by_column.at(i);
			for (int j = (int)sparks_in_column->size() - 1; j >= 0; j--)
			{
				auto spark = sparks_in_column->at(j);
				if (!spark->loop(hexes))
				{
					sparks_in_column->erase(sparks_in_column->begin() + j);
					delete spark;
				}
			}
		}
	}
	return true;
}
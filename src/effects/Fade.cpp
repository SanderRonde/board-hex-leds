#include "leds.h"
#include "effects/Fade.h"
#include "effects/EffectBase.h"

FADE_DIRECTION parse_direction_str(String direction_str)
{
	if (direction_str == "right")
	{
		return FADE_RIGHT;
	}
	else if (direction_str == "left")
	{
		return FADE_LEFT;
	}
	else if (direction_str == "top")
	{
		return FADE_TOP;
	}
	else if (direction_str == "bot")
	{
		return FADE_BOT;
	}
	throw std::exception();
}

std::map<int, float> Fade::calculate_pos_map(Hexes *hexes)
{
	std::map<int, float> led_pos_map;

	bool is_x = _direction == FADE_RIGHT || _direction == FADE_LEFT;
	int max_pos = (is_x ? hexes->get_x_max_pos() : hexes->get_y_max_pos()) + 1;
	for (int i = 0; i < hexes->num_hexes; i++)
	{
		auto hex = hexes->get_by_index(i);
		auto hex_pos = (is_x ? hexes->get_x_pos_for_index(hex->index) : hexes->get_y_pos_for_index(hex->index));
		for (int j = 0; j < hex->num_leds; j++)
		{
			float led_relative_pos = hex->get_relative_pos_for_index(j, is_x);
			float led_pos = (float)hex_pos + led_relative_pos;
			float super_relative_pos = led_pos / (float)max_pos;
			led_pos_map[hex->get_at_index(j)] = super_relative_pos;
		}
	}

	return led_pos_map;
}

Fade::Fade(Hexes *hexes, RequestObj *request) : EffectBase()
{
	_direction = parse_direction_str(request->stringv("direction"));
	_animate_from = request->has("animate_from") ? request->boolv("animate_from") : false;
	_animate_to = request->has("animate_to") ? request->boolv("animate_to") : false;

	if (_animate_from)
	{
		_from_interval_min = request->intv("from_interval_min");
		_from_interval_max = request->intv("from_interval_max");
		_from_color = Change::set_new_color_target(_from_color, _from_interval_min, _from_interval_max);
	}
	else
	{
		_from_color = Change::create_locked(request->colorv("from"));
	}

	if (_animate_to)
	{
		_to_interval_min = request->intv("to_interval_min");
		_to_interval_max = request->intv("to_interval_max");
		_to_color = Change::set_new_color_target(_to_color, _to_interval_min, _to_interval_max);
	}
	else
	{
		_to_color = Change::create_locked(request->colorv("to"));
	}

	_last_iteration = millis();
	_led_pos_map = calculate_pos_map(hexes);
}

CRGB fade_towards_color(CRGB from, CRGB to, float amount)
{
	return CRGB(
			floor(from.r * (1 - amount) + (to.r * amount)),
			floor(from.g * (1 - amount) + (to.g * amount)),
			floor(from.b * (1 - amount) + (to.b * amount)));
}

bool Fade::loop(Hexes *hexes)
{
	if (!_animate_from && !_animate_to && _rendered)
	{
		return false;
	}

	long long time_diff = millis() - _last_iteration;
	_last_iteration = millis();

	if (_animate_from)
	{
		_from_color.progress += time_diff;
		if (_from_color.progress > _from_color.total)
		{
			_from_color = Change::set_new_color_target(_from_color, _from_interval_min, _from_interval_max);
		}
	}
	if (_animate_to)
	{
		_to_color.progress += time_diff;
		if (_to_color.progress > _to_color.total)
		{
			_to_color = Change::set_new_color_target(_to_color, _to_interval_min, _to_interval_max);
		}
	}

	bool invert = _direction == FADE_BOT || _direction == FADE_LEFT;
	auto from_color = Change::get_current_color(_from_color);
	auto to_color = Change::get_current_color(_to_color);

	for (int i = 0; i < hexes->num_hexes; i++)
	{
		auto hex = hexes->get_by_index(i);
		for (int j = 0; j < hex->num_leds; j++)
		{
			float led_relative_pos = _led_pos_map[hex->get_at_index(j)];

			if (invert)
			{
				hex->set_at_index(j, fade_towards_color(to_color, from_color, led_relative_pos));
			}
			else
			{
				hex->set_at_index(j, fade_towards_color(from_color, to_color, led_relative_pos));
			}
		}
	}

	return true;
}
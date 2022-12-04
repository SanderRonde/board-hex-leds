#include "leds.h"
#include "effects/Fade.h"
#include "effects/EffectBase.h"
#include "color-util.h"

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

	bool is_x = _direction == FADE_LEFT || _direction == FADE_RIGHT;
	bool invert = _direction == FADE_BOT || _direction == FADE_LEFT;
	auto from_color = Change::get_current_color(_from_color);
	auto to_color = Change::get_current_color(_to_color);

	for (size_t i = 0; i < hexes->num_hexes; i++)
	{
		auto hex = hexes->get_by_index(i);
		for (size_t j = 0; j < hex->num_leds; j++)
		{
			float led_relative_pos = is_x ? hex->get_led_global_x_position_at_index(j) : hex->get_led_global_y_position_at_index(j);

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
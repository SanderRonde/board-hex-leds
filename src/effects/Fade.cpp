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
	else if (direction_str == "circular")
	{
		return FADE_CIRCULAR;
	}
	throw std::exception();
}

void Fade::calculate_distances_from_center(Hexes *hexes)
{
	float distances[NUM_LEDS];
	float min_distance = 1.0f;
	float max_distance = 0.0f;

	for (int i = 0; i < NUM_LEDS; i++)
	{
		float led_x_pos = hexes->get_led_x_pos_for_index(i) - 0.5f;
		float led_y_pos = hexes->get_led_y_pos_for_index(i) - 0.5f;

		// Distance from center is just pythagoras' theorem
		float distance_from_center = sqrt(pow(led_x_pos, 2) + pow(led_y_pos, 2));
		distances[i] = distance_from_center;

		min_distance = min(min_distance, distance_from_center);
		max_distance = max(max_distance, distance_from_center);
	}

	for (int i = 0; i < NUM_LEDS; i++)
	{
		distances_from_center[i] = (distances[i] - min_distance) / (max_distance - min_distance);
	}
}

Fade::Fade(Hexes *hexes) : EffectBase()
{

	calculate_distances_from_center(hexes);
	_last_iteration = millis();
}

FadeFromReq::FadeFromReq(Hexes *hexes, RequestObj *request) : Fade(hexes)
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
}

FadeManual::FadeManual(Hexes *hexes) : Fade(hexes)
{
	_direction = FADE_RIGHT;
	_animate_from = true;
	_animate_to = true;

	_from_interval_min = 10000;
	_from_interval_max = 20000;
	_from_color = Change::set_new_color_target(_from_color, _from_interval_min, _from_interval_max);

	_to_interval_min = 10000;
	_to_interval_max = 20000;
	_to_color = Change::set_new_color_target(_to_color, _to_interval_min, _to_interval_max);
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

	bool is_circular = _direction == FADE_CIRCULAR;
	bool is_x = _direction == FADE_LEFT || _direction == FADE_RIGHT;
	bool invert = _direction == FADE_BOT || _direction == FADE_LEFT;
	auto from_color = Change::get_current_color(_from_color);
	auto to_color = Change::get_current_color(_to_color);

	for (size_t i = 0; i < hexes->num_hexes; i++)
	{
		auto hex = hexes->get_by_index(i);
		for (size_t j = 0; j < hex->num_leds; j++)
		{
			float led_relative_pos;
			if (is_circular)
			{
				led_relative_pos = distances_from_center[hex->get_at_index(j)];
			}
			else if (is_x)
			{
				led_relative_pos = hex->get_led_global_x_position_at_index(j);
			}
			else
			{
				led_relative_pos = hex->get_led_global_y_position_at_index(j);
			}

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
#include "leds.h"
#include "effects/RandomColorsGradual.h"
#include "effects/EffectBase.h"

RandomColorsGradual::RandomColorsGradual(RequestObj *request) : EffectBase()
{
	_wait_time_min = request->intv("wait_time_min");
	_wait_time_max = request->intv("wait_time_max");
	_neighbour_influence = request->intv("neighbour_influence");
	_use_pastel = request->boolv("use_pastel");
	_use_split = request->boolv("use_split");
	for (int i = 0; i < HexNS::hexes->num_hexes; i++)
	{
		split_color_t prev;
		prev.first_half_color.next = random(0, MAX_CSHV_VALUE);
		prev.second_half_color.next = random(0, MAX_CSHV_VALUE);

		HexNS::Hex *hex = HexNS::hexes->get_by_index(i);
		_colors[i] = Split::set_new_color_target(prev, _wait_time_min, _wait_time_max, hex, _use_split, true);
	}
	_last_iteration = millis();
}

bool RandomColorsGradual::loop()
{
	int time_diff = millis() - _last_iteration;
	_last_iteration = millis();

	for (int i = 0; i < HexNS::hexes->num_hexes; i++)
	{
		Split::bump_progress(&_colors[i], time_diff);
		_colors[i] = Split::change_on_total_reached(_colors[i], _wait_time_min, _wait_time_max);

		HexNS::Hex *hex = _colors[i].hex;

		for (int j = 0; j < hex->num_leds; j++)
		{
			CRGB current_color = _use_pastel ? Split::get_current_color(_colors[i], j) : CHSV(Split::get_current_value(_colors[i], j), MAX_CSHV_VALUE, MAX_CSHV_VALUE);
			HexNS::Hex *neighbour = hex->get_neighbour_at_led(j);

			if (neighbour == NULL)
			{
				hex->set_at_index(j, current_color);
			}
			else
			{
				// Get the color of the neighbour
				int neighbour_led_idx = (j + hex->num_leds) % hex->num_leds;
				CRGB neighbour_color = _use_pastel ? Split::get_current_color(_colors[neighbour->index], neighbour_led_idx) : CHSV(Split::get_current_value(_colors[neighbour->index], neighbour_led_idx), MAX_CSHV_VALUE, MAX_CSHV_VALUE);

				// Move towards it a little
				hex->set_at_index(j, Util::fade_towards_color(current_color, neighbour_color, _neighbour_influence));
			}
		}
	}
	return true;
}
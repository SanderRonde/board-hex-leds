#include "effects.h"
#include "constants.h"
#include "hex.h"
#include "telnet.h"
#include "util.h"
#include "api.h"
#include "effects/Rainbow.h"
#include "effects/EffectBase.h"

Rainbow::Rainbow(RequestObj *request) : EffectBase()
{
	_revolve_step = Util::divide(MAX_CSHV_VALUE, request->intv("revolve_time"));
	_last_iteration = 0;
	_offset = 0;
}

bool Rainbow::loop(Hexes *hexes)
{
	long long time_diff = millis() - _last_iteration;
	double added_offset = (double)time_diff * _revolve_step;
	_offset = std::fmod(_offset + added_offset, MAX_CSHV_VALUE_MOD);
	_last_iteration = millis();

	int rounded_offset = (int)round(_offset);

	for (size_t i = 0; i < hexes->num_hexes; i++)
	{
		Hex *hex = hexes->get_by_index(i);
		int hex_led_step = (int)Util::divide(MAX_CSHV_VALUE_MOD, hex->num_leds);

		int total_step = rounded_offset;
		for (size_t j = 0; j < hex->num_leds; j++, total_step = (hex_led_step + total_step) % MAX_CSHV_VALUE_MOD)
		{
			hex->set_at_index(j, CHSV(total_step, MAX_CSHV_VALUE, MAX_CSHV_VALUE));
		}
	}
	return true;
}
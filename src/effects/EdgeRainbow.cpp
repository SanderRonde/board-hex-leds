#include "leds.h"
#include "effects/EdgeRainbow.h"
#include "effects/EffectBase.h"

EdgeRainbow::EdgeRainbow(RequestObj *request) : EffectBase()
{
	_revolve_step = Util::divide(MAX_CSHV_VALUE, request->intv("revolve_time"));
	_last_iteration = 0;
	_offset = 0;
}

bool EdgeRainbow::loop(Hexes *hexes)
{
	long long time_diff = millis() - _last_iteration;
	double added_offset = (double)time_diff * _revolve_step;
	_offset = std::fmod(_offset + added_offset, MAX_CSHV_VALUE_MOD);
	_last_iteration = millis();

	int rounded_offset = (int)floor(_offset);

	int total_offset = rounded_offset;
	for (int i = 0; i < NUM_LEDS; i++)
	{
		hexes->set_led_at_index(i, CHSV(total_offset, MAX_CSHV_VALUE, MAX_CSHV_VALUE));

		total_offset = (total_offset + RAINBOW_STEP) % MAX_CSHV_VALUE_MOD;
	}

	return true;
}
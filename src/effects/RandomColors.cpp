#include "leds.h"
#include "effects/RandomColors.h"
#include "effects/EffectBase.h"

RandomColors::RandomColors(RequestObj *request) : EffectBase()
{
	_wait_time = request->intv("wait_time");
}

bool RandomColors::loop()
{
	if (millis() - _last_iteration <= _wait_time)
	{
		return false;
	}

	for (int i = 0; i < HexNS::hexes->num_hexes; i++)
	{
		HexNS::Hex *hex = HexNS::hexes->get_by_index(i);

		hex->set_color(CHSV(random(0, MAX_CSHV_VALUE), MAX_CSHV_VALUE, MAX_CSHV_VALUE));
	}

	_last_iteration = millis();
	return true;
}
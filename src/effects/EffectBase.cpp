#include "effects/EffectBase.h"
#include "api.h"
#include "telnet.h"
#include "hex.h"

bool EffectBase::loop(Hexes *hexes)
{
	FastLED.showColor(CRGB::Black);
	return true;
}

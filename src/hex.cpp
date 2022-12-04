#include "hex.h"
#include "config.h"

namespace HexNS
{
	Hexes *setup()
	{
		return Hexes::from_described(USE_DESCRIBER(USED_LED_CONFIG), USE_DESCRIBER_NUM(USED_LED_CONFIG));
	}
};

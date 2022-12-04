#include "hex.h"
#include "config.h"

namespace HexNS
{
	Hexes *setup()
	{
		return Hexes::from_described(HEX_DESCRIBER, NUM_HEXES);
	}
};

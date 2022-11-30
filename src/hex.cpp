#include <hex.h>

namespace HexNS
{
	Hexes *setup()
	{
		return Hexes::from_described(described_hexes_desk, num_hexes_desk);
	}
};

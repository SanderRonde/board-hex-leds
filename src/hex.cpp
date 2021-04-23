#include <hex.h>
#include <stdlib.h>
#include <string.h>
#include <ArduinoJson.h>
#include <telnet.h>
#include <net.h>
#include <leds.h>
#include <hexes.h>

namespace HexNS
{
	void setup()
	{
		hexes = Hexes::from_json(______data_hexes_json, ______data_hexes_json_len);
	}

	Hexes *hexes;
};

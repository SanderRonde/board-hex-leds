#include <hex.h>
#include <stdlib.h>
#include <string.h>
#include <ArduinoJson.h>
#include <telnet.h>
#include <net.h>
#include <leds.h>

namespace HexNS
{
	void setup()
	{
		String result = Net::req("192.168.1.86", 1234, "/hexes.json");
		hexes = Hexes::from_json(result);
	}

	Hexes *hexes;
};


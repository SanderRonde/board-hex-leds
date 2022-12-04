#include <ArduinoOTA.h>
#include <Arduino.h>
#include "effects.h"
#include "config.h"
#include "api.h"
#include "telnet.h"
#include "leds.h"
#include "ota.h"
#include "net.h"
#include "hex.h"

void setup()
{
	Serial.begin(115200);
	Serial.println("Booting");

	// Setup telnet
	Telnet::setup(NAME);

	// Setup OTA and wait
	OTA::setup();
	LOGN("Booted. Waiting for possible OTAs");
	OTA::wait_for_otas();
	LOGN("Stopped waiting");

	// Setup the rest
	Net::setup();
	API::setup();
	Leds::setup();
	Hexes *hexes = HexNS::setup();
	Effects::setup(hexes);

	// Done
	LOGN("Booted");
	Serial.println("Booted");
	FastLED.showColor(CRGB::Black);
}

void loop()
{
	OTA::loop();
	// Telnet::loop();
	API::loop();

	Effects::loop();
}

#include <ArduinoOTA.h>
#include <Arduino.h>
#include <config.h>
#include <telnet.h>
#include <ota.h>
#include <net.h>

void setup() {
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
	// TODO:

	// Done
	LOGN("Booted");
}

void loop() {
	OTA::loop();
	Telnet::loop();
	
	//TODO:
}

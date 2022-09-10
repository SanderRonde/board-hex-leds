#include <string.h>

#ifndef MOCK
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#endif
#include "WiFiClient.a.h"
#include "Arduino.a.h"

#include <net_secrets.h>
#include <net.h>

using namespace std;

ESP8266WiFiMulti WiFiMulti;

namespace Net
{
	WiFiClient client;

	bool _setup = false;
	void setup()
	{
#ifdef MOCK
		return;
#endif

		if (_setup)
			return;

		for (uint8_t t = 4; t > 0; t--)
		{
			Serial.print("[SETUP] WAIT ");
			Serial.print(t);
			Serial.println("...");
			Serial.flush();
			delay(1000);
		}

		WiFi.mode(WIFI_AP_STA);
		WiFiMulti.addAP(WIFI_SSID, WIFI_PW);

		_setup = true;
	}

	void await_wifi()
	{
		while (WiFiMulti.run() != WL_CONNECTED)
		{
			delay(1000);
		}
	}
}
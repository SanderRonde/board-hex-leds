#pragma once

#include <ESP***REMOVED***WiFi.h>
#include <ESP***REMOVED***WiFiMulti.h>
#include <ESP***REMOVED***HTTPClient.h>
#include <ESP***REMOVED***WebServer.h>
#include <WiFiClient.h>
#include <FastLED.h>
#include <string.h>

class RequestObj
{
private:
	ESP***REMOVED***WebServer *_server;
	CRGB _parse_color(String color);

public:
	RequestObj(ESP***REMOVED***WebServer *server);
	int intv(const char *arg_name, int default_val = -999);
	bool boolv(const char *arg_name);
	CRGB colorv(const char *arg_name, const char *default_val = "");
};

namespace API
{
	void setup();

	void loop();
}
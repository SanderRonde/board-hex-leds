#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <FastLED.h>
#include <string.h>

class RequestObj
{
private:
	ESP8266WebServer *_server;
	CRGB _parse_color(String color);
	void _err(const char* arg_name);

public:
	RequestObj(ESP8266WebServer *server);
	int intv(const char *arg_name, int default_val = -999);
	String stringv(const char *arg_name);
	bool assert_has(std::initializer_list<const char*> list);
	bool has(const char* arg_name);
	bool boolv(const char *arg_name);
	CRGB colorv(const char *arg_name, const char *default_val = "");
};

namespace API
{
	void setup();

	void loop();
}
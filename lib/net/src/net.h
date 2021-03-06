#pragma once

#include <string.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

using namespace std;

namespace Net
{
	String req(const char *host, int port, const char *path);

	String req_auth(const char *host, int port, const char *path);

	void setup();

	void await_wifi();
}
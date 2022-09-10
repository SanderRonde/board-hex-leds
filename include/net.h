#pragma once

#include <string.h>
#include <string>

#ifndef MOCK
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#endif
#include "WiFiClient.a.h"

using namespace std;

namespace Net
{
	void setup();

	void await_wifi();
}
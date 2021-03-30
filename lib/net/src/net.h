#pragma once

#include <string.h>

#include <ESP***REMOVED***WiFi.h>
#include <ESP***REMOVED***WiFiMulti.h>
#include <ESP***REMOVED***HTTPClient.h>
#include <ESP***REMOVED***WebServer.h>
#include <WiFiClient.h>

using namespace std;

namespace Net
{
	String req(const char *host, int port, const char *path);

	String req_auth(const char *host, int port, const char *path);

	void setup();

	void await_wifi();
}
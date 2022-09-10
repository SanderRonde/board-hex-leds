#pragma once

#ifndef MOCK
#include "WiFiClient.h"
#else
#include <string>
#include "Arduino.a.h"

class WiFiClient
{
public:
	void connect(const char *ip, int port);
	bool connected();
	void keepAlive();
	size_t write(const char *str);
	void println(const char *str);
	void printf(const char *format, ...);
};

typedef enum
{
	WL_NO_SHIELD = 255, // for compatibility with WiFi Shield library
	WL_IDLE_STATUS = 0,
	WL_NO_SSID_AVAIL = 1,
	WL_SCAN_COMPLETED = 2,
	WL_CONNECTED = 3,
	WL_CONNECT_FAILED = 4,
	WL_CONNECTION_LOST = 5,
	WL_WRONG_PASSWORD = 6,
	WL_DISCONNECTED = 7
} wl_status_t;

class ESP8266WiFiMulti
{
public:
	void addAP(const char *ip, const char *pw);
	wl_status_t run();
};

#define WIFI_AP_STA 1
class WiFiCls
{
public:
	void mode(int sta);
};

enum HTTPMethod
{
	HTTP_ANY,
	HTTP_GET,
	HTTP_HEAD,
	HTTP_POST,
	HTTP_PUT,
	HTTP_PATCH,
	HTTP_DELETE,
	HTTP_OPTIONS
};

class ESP8266WebServer
{
public:
	ESP8266WebServer(int port);
	void handleClient();

	void begin(int port);
	String arg(String arg);
	bool hasArg(String arg);
	void send(int code, const char* content_type, const char* data);
};

extern WiFiCls WiFi;

#endif
#ifdef MOCK

#include "WiFiClient.a.h"

void WiFiClient::connect(const char *ip, int port)
{
}
bool WiFiClient::connected()
{
	return true;
}
void WiFiClient::keepAlive()
{
}
size_t WiFiClient::write(const char *str)
{
	::printf("WiFiClient writing: %s\n", str);
	return 10;
}

void WiFiClient::println(const char *str)
{
	::printf("Telnet writing %s\n", str);
}

void WiFiClient::printf(const char *format, ...)
{
	::printf(format);
}

void ESP8266WiFiMulti::addAP(const char *ip, const char *pw) {}
wl_status_t ESP8266WiFiMulti::run() { return WL_CONNECTED; }

void WiFiCls::mode(int sta){};



ESP8266WebServer::ESP8266WebServer(int port){};
void ESP8266WebServer::handleClient(){};
void ESP8266WebServer::begin(int port){};
bool ESP8266WebServer::hasArg(String arg){return true;};
String ESP8266WebServer::arg(String arg){return String("");};
void ESP8266WebServer::send(int code, const char* content_type, const char* data){}

WiFiCls WiFi;

#endif
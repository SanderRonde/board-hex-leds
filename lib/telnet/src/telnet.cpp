#include <WiFiClient.h>
#include <secrets.h>
#include <Arduino.h>
#include <string.h>
#include <telnet.h>
#include <net.h>

#define TELNET_PORT 23

#define RETRY_TIME 30

namespace Telnet {
	WiFiClient client;

	char* _name = (char*) malloc(sizeof(char) * 50);
	void connect() {
		client.connect(TELNET_BASE_NAME, TELNET_PORT);
		client.write("set-name:");
		client.write(_name);
		client.write("\n");
		client.write("Hi\n");
		client.write(("Reset reason: " + ESP.getResetReason() + "\n").c_str());
	}

	void setup(const char* name) {
		strcpy(_name, name);

		Net::setup();
		Net::await_wifi();

		connect();
	}

	unsigned long last_connect = millis();
	void loop() {
		if (millis() - last_connect > RETRY_TIME * 1000) {
			client.keepAlive();
			if (!client.connected()) {
				connect();
			}
		}
	}
}
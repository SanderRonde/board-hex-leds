#include <net.h>

namespace API
{
	ESP8266WebServer server(80);

	void handle_root()
	{
		server.send(200, "text/plain", "Hi!");
	}

	void handle_not_found()
	{
		server.send(404, "text/plain", "404: Not found");
	}

	void handle_set_led()
	{
		if (!server.hasArg("num") ||
				(!server.hasArg("color") && !server.hasArg("power")))
		{
			server.send(400, "text/plain", "400: Invalid reiquest");
			return;
		}

		// TODO:
	}

	void setup()
	{
		server.on("/", HTTP_GET, handle_root);
		server.on("/set_led", HTTP_POST, handle_set_led);
		server.onNotFound(handle_not_found);
	}

	void loop()
	{
		server.handleClient();
	}
}
#include <net.h>
#include <effects.h>

#define DEFAULT_REVOLVE_TIME 10000
#define DEFAULT_RANDOM_COLORS_GRADUAL_WAIT_TIME 1000
#define DEFAULT_RANDOM_COLORS_WAIT_TIME 1000

namespace API
{
	ESP***REMOVED***WebServer server(80);

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

		if (server.hasArg("power") && server.arg("power") == "off")
		{
			Effects::Effects::set_led(server.arg("num"), "#000000");
		}
		else
		{
			Effects::Effects::set_led(server.arg("num"), server.arg("color"));
		}
	}

	void handle_set_led_in_hex()
	{
		if (!server.hasArg("index") || !server.hasArg("hex_id") ||
				(!server.hasArg("color") && !server.hasArg("power")))
		{
			server.send(400, "text/plain", "400: Invalid reiquest");
			return;
		}

		if (server.hasArg("power") && server.arg("power") == "off")
		{
			Effects::Effects::set_led_in_hex(server.arg("index"), server.arg("hex_id"), "#000000");
		}
		else
		{
			Effects::Effects::set_led_in_hex(server.arg("index"), server.arg("hex_id"), server.arg("color"));
		}
	}

	void handle_set_hex()
	{
		if (!server.hasArg("hex_id") ||
				(!server.hasArg("color") && !server.hasArg("power")))
		{
			server.send(400, "text/plain", "400: Invalid reiquest");
			return;
		}

		if (server.hasArg("power") && server.arg("power") == "off")
		{
			Effects::Effects::set_hex(server.arg("hex_id"), "#000000");
		}
		else
		{
			Effects::Effects::set_hex(server.arg("hex_id"), server.arg("color"));
		}
	}

	void handle_set_all()
	{
		if ((!server.hasArg("color") && !server.hasArg("power")))
		{
			server.send(400, "text/plain", "400: Invalid reiquest");
			return;
		}

		if (server.hasArg("power") && server.arg("power") == "off")
		{
			Effects::Effects::set_all("#000000");
		}
		else
		{
			Effects::Effects::set_all(server.arg("color"));
		}
	}

	void handle_set_rainbow()
	{
		int revolve_time = DEFAULT_REVOLVE_TIME;
		if (server.hasArg("revolve_time"))
		{
			String revolve_time_str = server.arg("revolve_time");
			revolve_time = atoi(revolve_time_str.c_str());
		}

		Effects::Effects::enable_rainbow(revolve_time);
	}

		void handle_set_edge_rainbow()
	{
		int revolve_time = DEFAULT_REVOLVE_TIME;
		if (server.hasArg("revolve_time"))
		{
			String revolve_time_str = server.arg("revolve_time");
			revolve_time = atoi(revolve_time_str.c_str());
		}

		Effects::Effects::enable_edge_rainbow(revolve_time);
	}

	void handle_set_move()
	{
		Effects::Effects::move_around();
	}

	void handle_set_random_colors_gradual()
	{
		int wait_time = DEFAULT_RANDOM_COLORS_GRADUAL_WAIT_TIME;
		if (server.hasArg("wait_time"))
		{
			String wait_time_str = server.arg("wait_time");
			wait_time = atoi(wait_time_str.c_str());
		}

		Effects::Effects::random_colors_gradual(wait_time);
	}

	void handle_set_random_colors()
	{
		int wait_time = DEFAULT_RANDOM_COLORS_WAIT_TIME;
		if (server.hasArg("wait_time"))
		{
			String wait_time_str = server.arg("wait_time");
			wait_time = atoi(wait_time_str.c_str());
		}

		Effects::Effects::random_colors(wait_time);
	}

	void setup()
	{

		server.on("/", HTTP_GET, handle_root);
		server.on("/set_led_in_hex", HTTP_POST, handle_set_led_in_hex);
		server.on("/set_led", HTTP_POST, handle_set_led);
		server.on("/set_hex", HTTP_POST, handle_set_hex);
		server.on("/set_all", HTTP_POST, handle_set_all);
		server.on("/effects/rainbow", HTTP_POST, handle_set_rainbow);
		server.on("/effects/edge_rainbow", HTTP_POST, handle_set_edge_rainbow);
		server.on("/effects/move", HTTP_POST, handle_set_move);
		server.on("/effects/random_colors_gradual", HTTP_POST, handle_set_random_colors_gradual);
		server.on("/effets/random_colors", HTTP_POST, handle_set_random_colors);
		server.onNotFound(handle_not_found);
	}

	void loop()
	{
		server.handleClient();
	}
}
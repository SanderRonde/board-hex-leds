#include <net.h>
#include <effects.h>
#include <telnet.h>

#define DEFAULT_REVOLVE_TIME 10000
#define DEFAULT_RANDOM_COLORS_GRADUAL_WAIT_TIME 1000
#define DEFAULT_RANDOM_COLORS_WAIT_TIME 1000
#define SERVER_PORT 80

namespace API
{
	ESP***REMOVED***WebServer server(80);

	inline void respond_succes()
	{
		server.send(200, "text/json", "{\"success\": true}");
	}

	inline void log_request()
	{
		LOGF("Got request for URI: %s\n", server.uri().c_str());
	}

	void handle_root()
	{
		log_request();
		server.send(200, "text/plain", "Hi!");
	}

	void handle_not_found()
	{
		log_request();
		server.send(404, "text/plain", "404: Not found");
	}

	void handle_set_led()
	{
		log_request();
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

		respond_succes();
	}

	void handle_set_led_in_hex()
	{
		log_request();
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

		respond_succes();
	}

	void handle_set_hex()
	{
		log_request();
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

		respond_succes();
	}

	void handle_set_all()
	{
		log_request();
		if ((!server.hasArg("color") && !server.hasArg("power")))
		{
			server.send(400, "text/plain", "400: Invalid request");
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

		respond_succes();
	}

	void handle_set_rainbow()
	{
		log_request();
		int revolve_time = DEFAULT_REVOLVE_TIME;
		if (server.hasArg("revolve_time"))
		{
			String revolve_time_str = server.arg("revolve_time");
			revolve_time = atoi(revolve_time_str.c_str());
		}

		Effects::Effects::enable_rainbow(revolve_time);

		respond_succes();
	}

	void handle_set_edge_rainbow()
	{
		log_request();
		int revolve_time = DEFAULT_REVOLVE_TIME;
		if (server.hasArg("revolve_time"))
		{
			String revolve_time_str = server.arg("revolve_time");
			revolve_time = atoi(revolve_time_str.c_str());
		}

		Effects::Effects::enable_edge_rainbow(revolve_time);

		respond_succes();
	}

	void handle_set_move()
	{
		log_request();
		Effects::Effects::move_around();

		respond_succes();
	}

	void handle_set_random_colors_gradual()
	{
		log_request();
		if (!server.hasArg("wait_time_min") || !server.hasArg("wait_time_max") || !server.hasArg("neighbour_influence") || !server.hasArg("use_pastel"))
		{
			server.send(400, "text/plain", "400: Invalid request");
			return;
		}

		int wait_time_min = atoi(server.arg("wait_time_min").c_str());
		int wait_time_max = atoi(server.arg("wait_time_max").c_str());
		int neighbour_influence = atoi(server.arg("neighbour_influence").c_str());
		bool use_pastel = server.arg("use_pastel") == "true";
		Effects::Effects::random_colors_gradual(wait_time_min, wait_time_max, neighbour_influence, use_pastel);

		respond_succes();
	}

	void handle_set_random_colors()
	{
		log_request();
		int wait_time = DEFAULT_RANDOM_COLORS_WAIT_TIME;
		if (server.hasArg("wait_time"))
		{
			String wait_time_str = server.arg("wait_time");
			wait_time = atoi(wait_time_str.c_str());
		}

		Effects::Effects::random_colors(wait_time);

		respond_succes();
	}

	void handle_off()
	{
		log_request();
		Effects::disable();
		respond_succes();
	}

	void handle_on()
	{
		log_request();
		Effects::enable();
		respond_succes();
	}

	void setup()
	{

		server.begin(SERVER_PORT);
		server.on("/", HTTP_GET, handle_root);
		server.on("/on", HTTP_POST, handle_on);
		server.on("/off", HTTP_POST, handle_off);
		server.on("/set_led_in_hex", HTTP_POST, handle_set_led_in_hex);
		server.on("/set_led", HTTP_POST, handle_set_led);
		server.on("/set_hex", HTTP_POST, handle_set_hex);
		server.on("/set_all", HTTP_POST, handle_set_all);
		server.on("/effects/rainbow", HTTP_POST, handle_set_rainbow);
		server.on("/effects/edge_rainbow", HTTP_POST, handle_set_edge_rainbow);
		server.on("/effects/move", HTTP_POST, handle_set_move);
		server.on("/effects/random_colors_gradual", HTTP_POST, handle_set_random_colors_gradual);
		server.on("/effects/random_colors", HTTP_POST, handle_set_random_colors);
		server.onNotFound(handle_not_found);

		LOGF("Server listening on port %d\n", SERVER_PORT);
	}

	void loop()
	{
		server.handleClient();
	}
}
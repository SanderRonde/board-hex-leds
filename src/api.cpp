#include <net.h>
#include <effects.h>
#include <telnet.h>

#define SERVER_PORT 80

namespace API
{
	ESP***REMOVED***WebServer server(80);

	inline void respond_succes()
	{
		server.send(200, "text/json", "{\"success\": true}");
	}

	inline void respond_invalid_request()
	{
		server.send(400, "text/plain", "400: Invalid request");
	}

	inline int get_num_arg(String arg_name)
	{
		return atoi(server.arg(arg_name).c_str());
	}

	bool has_single_arg(const char *arg)
	{
		if (arg == NULL)
			return true;
		return server.hasArg(String(arg));
	}

		bool require_args(const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char* arg5)
	{
		if (!has_single_arg(arg1) || !has_single_arg(arg2) || !has_single_arg(arg3) || !has_single_arg(arg4) || !has_single_arg(arg5))
		{
			respond_invalid_request();
			return false;
		}
		return true;
	}

	bool require_args(const char *arg1, const char *arg2, const char *arg3, const char *arg4)
	{
		return require_args(arg1, arg2, arg3, arg4, NULL);
	}

	bool require_args(const char *arg1, const char *arg2, const char *arg3)
	{
		return require_args(arg1, arg2, arg3, NULL);
	}

	bool require_args(const char *arg1, const char *arg2)
	{
		return require_args(arg1, arg2, NULL);
	}

	bool require_args(const char *arg1)
	{
		return require_args(arg1, NULL);
	}

	inline void log_request()
	{
		LOGF("Got request for URI: %s with %d args\n", server.uri().c_str(), server.args());
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
			respond_invalid_request();
			return;
		}

		if (server.hasArg("power") && server.arg("power") == "off")
		{
			Effects::Effects::set_led(get_num_arg("num"), "#000000");
		}
		else
		{
			Effects::Effects::set_led(get_num_arg("num"), server.arg("color"));
		}

		respond_succes();
	}

	void handle_set_led_in_hex()
	{
		log_request();
		if (!server.hasArg("index") || !server.hasArg("hex_id") ||
				(!server.hasArg("color") && !server.hasArg("power")))
		{
			respond_invalid_request();
			return;
		}

		if (server.hasArg("power") && server.arg("power") == "off")
		{
			Effects::Effects::set_led_in_hex(get_num_arg("index"), get_num_arg("hex_id"), "#000000");
		}
		else
		{
			Effects::Effects::set_led_in_hex(get_num_arg("index"), get_num_arg("hex_id"), server.arg("color"));
		}

		respond_succes();
	}

	void handle_set_hex()
	{
		log_request();
		if (!server.hasArg("hex_id") ||
				(!server.hasArg("color") && !server.hasArg("power")))
		{
			respond_invalid_request();
			return;
		}

		if (server.hasArg("power") && server.arg("power") == "off")
		{
			Effects::Effects::set_hex(get_num_arg("hex_id"), "#000000");
		}
		else
		{
			Effects::Effects::set_hex(get_num_arg("hex_id"), server.arg("color"));
		}

		respond_succes();
	}

	void handle_set_all()
	{
		log_request();
		if (!require_args("color", "power"))
			return;

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
		if (!require_args("revolve_time"))
			return;

		Effects::Effects::enable_rainbow(get_num_arg("revolve_time"));

		respond_succes();
	}

	void handle_set_edge_rainbow()
	{
		log_request();

		if (!require_args("revolve_time"))
			return;

		Effects::Effects::enable_edge_rainbow(get_num_arg("revolve_time"));

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
		if (!require_args("wait_time_min", "wait_time_max", "neighbour_influence", "use_pastel", "use_split"))
			return;

		int wait_time_min = get_num_arg("wait_time_min");
		int wait_time_max = get_num_arg("wait_time_max");
		int neighbour_influence = get_num_arg("neighbour_influence");
		bool use_pastel = server.arg("use_pastel") == "true";
		bool use_split = server.arg("use_split") == "true";
		Effects::Effects::random_colors_gradual(wait_time_min, wait_time_max, neighbour_influence, use_pastel, use_split);

		respond_succes();
	}

	void handle_set_random_colors()
	{
		log_request();
		if (!require_args("wait_time"))
			return;

		Effects::Effects::random_colors(get_num_arg("wait_time"));

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
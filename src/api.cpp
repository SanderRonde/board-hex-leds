#define SERVER_PORT 80
#include <telnet.h>

#include <net.h>
#include <effects.h>
#include <FastLED.a.h>
#include <string>

CRGB RequestObj::_parse_color(String color)
{
	CRGB color_obj;
	char red[3];
	char green[3];
	char blue[3];

	const char *color_c_str = color.c_str();
	strncpy(red, color_c_str + 1, 2);
	strncpy(green, color_c_str + 3, 2);
	strncpy(blue, color_c_str + 5, 2);
	red[2] = green[2] = blue[2] = '\n';
	color_obj.r = strtol(red, NULL, 16);
	color_obj.g = strtol(green, NULL, 16);
	color_obj.b = strtol(blue, NULL, 16);

	return color_obj;
}

RequestObj::RequestObj(ESP8266WebServer *server) : _server(server) {}

void RequestObj::_err(const char *arg_name)
{
	std::string err = "Argument ";
	err += arg_name;
	err += " is missing";
	Serial.println(err.c_str());
	LOGF("%s\n", err.c_str());
	_server->send(400, "text/plain", err.c_str());
}

int RequestObj::intv(const char *arg_name, int default_val)
{
	if (!_server->hasArg(String(arg_name)))
	{
		if (default_val != -999)
		{
			return default_val;
		}
		_err(arg_name);
		return 1;
	}

	return atoi(_server->arg(arg_name).c_str());
}

String RequestObj::stringv(const char *arg_name)
{
	if (!_server->hasArg(String(arg_name)))
	{
		_err(arg_name);
		return "";
	}

	return _server->arg(arg_name);
}

bool RequestObj::has(const char *arg_name)
{
	return _server->hasArg(String(arg_name));
}

bool RequestObj::boolv(const char *arg_name)
{
	if (!_server->hasArg(String(arg_name)))
	{
		_err(arg_name);
		return false;
	}

	return _server->arg(arg_name) == "true";
}

CRGB RequestObj::colorv(const char *arg_name, const char *default_val)
{
	if (!_server->hasArg(String(arg_name)))
	{
		if (strlen(default_val) != 0)
		{
			return _parse_color(default_val);
		}
		_err(arg_name);
		return CRGB::Black;
	}

	return _parse_color(_server->arg(arg_name).c_str());
}

class ResponseObj
{
private:
	ESP8266WebServer *_server;

public:
	ResponseObj(ESP8266WebServer *server) : _server(server) {}

public:
	int success()
	{
		_server->send(200, "application/json", "{\"success\": true}");
		return 0;
	}

	int success_plain(std::string response)
	{
		_server->send(200, "text/plain", response.c_str());
		return 0;
	}

	int success_json(std::string response)
	{
		_server->send(200, "application/json", response.c_str());
		return 0;
	}

	int not_found()
	{
		_server->send(404, "text/plain", "Not found");
		return 0;
	}

	int err(std::string err)
	{
		_server->send(400, "text/plain", err.c_str());
		return 0;
	}
};

class RequestDataContainer
{
public:
	ESP8266WebServer *server;
	RequestObj *request;
	ResponseObj *response;

	RequestDataContainer(ESP8266WebServer *server_,
											 RequestObj *request_,
											 ResponseObj *response_) : server(server_), request(request_), response(response_)
	{
	}
};

class APIRequestHandler
{
protected:
	RequestObj *_request;
	ResponseObj *_response;
	ESP8266WebServer *_server;
#ifndef MOCK
	static std::function<void()> _route(RequestDataContainer *self, int (*handler)(RequestObj *, ResponseObj *))
	{
		return [=]() -> void
		{
			LOGF("Got request for URI: %s with %d args\n", self->server->uri().c_str(), self->server->args());
			for (int i = 0; i < self->server->args(); i++)
			{
				LOGF("\t[%s]: %s\n", self->server->argName(i).c_str(), self->server->arg(i).c_str());
			}
			handler(self->request, self->response);
		};
	}
#endif

public:
	APIRequestHandler(ESP8266WebServer *server) : _server(server)
	{
		_request = new RequestObj(server);
		_response = new ResponseObj(server);
	};
	~APIRequestHandler()
	{
		free(_request);
		free(_response);
	};
};

class RequestHandlerWithRoutes : protected APIRequestHandler
{
private:
	RequestDataContainer *_container;

	static int _route_root(RequestObj *request, ResponseObj *response)
	{
		return response->success_plain("Hi!");
	}

	static int _route_is_on(RequestObj *request, ResponseObj *response)
	{
		bool is_enabled = Effects::is_enabled();
		std::string enabled_str = is_enabled ? "true" : "false";
		std::string json = "{\"enabled\": " + enabled_str + "}";
		return response->success_json(json);
	}

	static int _route_off(RequestObj *request, ResponseObj *response)
	{
		Effects::disable();
		return response->success();
	}

	static int _route_on(RequestObj *request, ResponseObj *response)
	{
		Effects::enable();
		return response->success();
	}

	static int _route_set_led_in_hex(RequestObj *request, ResponseObj *response)
	{
		Effects::Effects::set_led_in_hex(request);
		return response->success();
	}

	static int _route_set_led(RequestObj *request, ResponseObj *response)
	{
		Effects::Effects::set_led(request);
		return response->success();
	}

	static int _route_set_hex(RequestObj *request, ResponseObj *response)
	{
		Effects::Effects::set_hex(request);
		return response->success();
	}

	static int _route_set_all(RequestObj *request, ResponseObj *response)
	{
		Effects::Effects::set_all(request);
		return response->success();
	}

	static int _route_set_rainbow(RequestObj *request, ResponseObj *response)
	{
		Effects::Effects::enable_rainbow(request);
		return response->success();
	}

	static int _route_set_edge_rainbow(RequestObj *request, ResponseObj *response)
	{
		Effects::Effects::enable_edge_rainbow(request);
		return response->success();
	}

	static int _route_set_move(RequestObj *request, ResponseObj *response)
	{
		Effects::Effects::move_around();
		return response->success();
	}

	static int _route_set_random_colors_gradual(RequestObj *request, ResponseObj *response)
	{
		Effects::Effects::random_colors_gradual(request);
		return response->success();
	}

	static int _route_set_random_colors(RequestObj *request, ResponseObj *response)
	{
		Effects::Effects::random_colors(request);
		return response->success();
	}

	static int _route_fade(RequestObj *request, ResponseObj *response)
	{
		Effects::Effects::fade(request);
		return response->success();
	}

	static int _route_not_found(RequestObj *request, ResponseObj *response)
	{
		return response->not_found();
	}

public:
	RequestHandlerWithRoutes(ESP8266WebServer *server) : APIRequestHandler(server)
	{
		_container = new RequestDataContainer(_server, _request, _response);
	}

	~RequestHandlerWithRoutes()
	{
		free(_container);
	}

	void listen()
	{
#ifndef MOCK
		_server->begin(SERVER_PORT);
		_server->on("/", HTTP_GET, _route(_container, _route_root));
		_server->on("/is_on", HTTP_POST, _route(_container, _route_is_on));
		_server->on("/on", HTTP_POST, _route(_container, _route_on));
		_server->on("/off", HTTP_POST, _route(_container, _route_off));
		_server->on("/set_led_in_hex", HTTP_POST, _route(_container, _route_set_led_in_hex));
		_server->on("/set_led", HTTP_POST, _route(_container, _route_set_led));
		_server->on("/set_hex", HTTP_POST, _route(_container, _route_set_hex));
		_server->on("/set_all", HTTP_POST, _route(_container, _route_set_all));
		_server->on("/effects/rainbow", HTTP_POST, _route(_container, _route_set_rainbow));
		_server->on("/effects/edge_rainbow", HTTP_POST, _route(_container, _route_set_edge_rainbow));
		_server->on("/effects/move", HTTP_POST, _route(_container, _route_set_move));
		_server->on("/effects/random_colors_gradual", HTTP_POST, _route(_container, _route_set_random_colors_gradual));
		_server->on("/effects/random_colors", HTTP_POST, _route(_container, _route_set_random_colors));
		_server->on("/effects/fade", HTTP_POST, _route(_container, _route_fade));
		_server->onNotFound(_route(_container, _route_not_found));
#endif
	}
};

namespace API
{
	ESP8266WebServer server(SERVER_PORT);
	RequestHandlerWithRoutes router(&server);

	void setup()
	{
		router.listen();
		LOGF("Server listening on port %d\n", SERVER_PORT);
	}

	void loop()
	{
		server.handleClient();
	}
}

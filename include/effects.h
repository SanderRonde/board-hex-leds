namespace Effects
{
	namespace Effects
	{
		void set_led(String index, String color);

		void set_led_in_hex(String index, String hex_id, String color);

		void set_hex(String hex_id, String color);

		void enable_rainbow(int revolve_time);

		void enable_edge_rainbow(int revolve_time);

		void set_all(String str_color);

		void move_around();

		void random_colors_gradual(int wait_time_min, int wait_time_max, int neighbour_influence, bool use_pastel);

		void random_colors(int wait_time);
	}

	void enable();

	void disable();
	
	void loop();
}
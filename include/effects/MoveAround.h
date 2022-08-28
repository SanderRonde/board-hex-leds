#pragma once

#include "effects/EffectBase.h"
#include "effects/change.h"
#include "effects.h"
#include "constants.h"
#include "hex.h"
#include "util.h"
#include "api.h"

struct high_intensity_hex
{
	int index;

	change_t move_change;
	hex_side_t move_side;

	change_t color_change;
};

class MoveAround : public EffectBase
{
private:
	change_t _background_colors[HEXES_UPPERBOUND];

	high_intensity_hex _high_intensity_hexes[NUM_HIGH_INTENSITY_HEXES];
	long long _last_iteration;

	int random_except(int min, int max, int except);
	int choose_new_move_direction(Hexes *hexes, int current_target);
	change_t set_new_move_target(Hexes *hexes, int current_id);
	int get_direction_angle(hex_side_t direction);
	int mod_positive(int base, int modulo);
	int get_distance_between_led_and_center(float center_progress, int center_angle, int led_angle);

public:
	MoveAround(Hexes *hexes);
	bool loop(Hexes *hexes) override;
};
#pragma once

#include "effects/EffectBase.h"
#include "constants.h"
#include "hex.h"
#include "util.h"
#include "api.h"
#include "split.h"

class RandomColorsGradual : public EffectBase
{
private:
	int _wait_time_min = 0;
	int _wait_time_max = 0;
	uint8_t _neighbour_influence = 0;
	bool _use_pastel = false;
	bool _use_split = false;
	long long _last_iteration;
	long long _last_refresh_iteration;
	split_color_t _colors[HEXES_UPPERBOUND];

public:
	RandomColorsGradual(RequestObj *request);
	bool loop() override;
};
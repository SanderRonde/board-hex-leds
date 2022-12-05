#pragma once

#include "effects/EffectBase.h"
#include "effects/change.h"
#include "constants.h"
#include "hex.h"
#include "util.h"
#include "api.h"

enum FADE_DIRECTION
{
	FADE_RIGHT,
	FADE_LEFT,
	FADE_TOP,
	FADE_BOT,
	FADE_CIRCULAR,
};

class Fade : public EffectBase
{
protected:
	change_t _from_color = {0};
	change_t _to_color = {0};
	FADE_DIRECTION _direction;
	bool _rendered = false;
	float distances_from_center[NUM_LEDS] = {0};

	bool _animate_from;
	bool _animate_to;
	int _from_interval_min;
	int _from_interval_max;
	int _to_interval_min;
	int _to_interval_max;

	unsigned long _last_iteration;
	void calculate_distances_from_center(Hexes *hexes);

public:
	Fade(Hexes* hexes);
	std::map<int, float> calculate_pos_map(Hexes* hexes);
	bool loop(Hexes *hexes) override;
};

class FadeFromReq : public Fade
{
public:
	FadeFromReq(Hexes* hexes, RequestObj *request);
};

class FadeManual : public Fade
{
public:
	FadeManual(Hexes* hexes);
};
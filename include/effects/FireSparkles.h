#pragma once

#include "effects/EffectBase.h"
#include "constants.h"
#include "hex.h"
#include "util.h"
#include "api.h"

#define MAX_NUM_SPARKS 500

struct spark_t
{
	bool empty;
	uint16_t led_index;
	byte progress;
	bool past_peak;
};

class FireSparkles : public EffectBase
{
private:
	spark_t _sparks[MAX_NUM_SPARKS];
	unsigned long _last_spark_iteration;

public:
	FireSparkles(Hexes *hexes);
	bool loop(Hexes *hexes) override;
};
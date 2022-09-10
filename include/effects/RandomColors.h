#pragma once

#include "effects/EffectBase.h"
#include "constants.h"
#include "hex.h"
#include "util.h"
#include "api.h"

class RandomColors : public EffectBase
{
private:
	int _wait_time = 0;
	unsigned long _last_iteration;

public:
	RandomColors(RequestObj *request);
	bool loop(Hexes *hexes) override;
};
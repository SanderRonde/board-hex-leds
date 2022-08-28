#pragma once

#include "effects/EffectBase.h"
#include "constants.h"
#include "hex.h"
#include "util.h"
#include "api.h"

class Rainbow : public EffectBase
{
private:
	long long _last_iteration;
	double _revolve_step;
	double _offset;
	int _num_hexes;

public:
	Rainbow(RequestObj *request);
	bool loop(Hexes *hexes) override;
};
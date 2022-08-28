#pragma once

#include "api.h"
#include "hex.h"

class EffectBase
{
public:
	EffectBase() {}

	virtual bool loop(Hexes *hexes);
};
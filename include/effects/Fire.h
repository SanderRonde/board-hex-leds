#pragma once

#include "effects/EffectBase.h"
#include "constants.h"
#include "hex.h"
#include "util.h"
#include "api.h"

class Spark
{
private:
public:
	std::vector<Hex *>* column;
	/** Starts at 0, moves to 255 */
	byte progress;
	bool disabled;
	int heat;

	Spark(std::vector<Hex *>* column);
	bool loop(Hexes *hexes);
};

class Fire : public EffectBase
{
private:
	std::vector<std::vector<Hex *>> _columns;
	std::map<int, std::vector<Spark *>*> _sparks_by_column;
	unsigned long _last_spark_iteration;

public:
	Fire(Hexes *hexes);
	bool loop(Hexes *hexes) override;
};
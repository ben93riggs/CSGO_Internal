#pragma once
#include "Singleton.h"
#include "SourceEngine/Vector.hpp"

class Knifebot : public singleton<Knifebot>
{
	Vector HeadMins = Vector(-16, -16, -18);
	Vector HeadMaxs = Vector(16, 16, 18);

	bool CanHit(bool& frombehind, bool stab);

public:
	void CreateMove();

};

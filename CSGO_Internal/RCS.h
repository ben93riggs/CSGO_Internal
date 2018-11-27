#pragma once
#include "Singleton.h"
#include "SourceEngine/Vector.hpp"

class C_CSPlayer;

class RCS : public singleton<RCS>
{
	int shotsFired = 0;
	int oldShotsFired = 0;
	Vector oldPunchAngles = Vector(0, 0, 0);
	Vector finalVec = Vector(0, 0, 0);

	void LegitRCS(C_CSPlayer* pLocal);

	void SilentRCS(bool &bret, C_CSPlayer* pLocal);

public:
	void CreateMove(bool &bret);

};

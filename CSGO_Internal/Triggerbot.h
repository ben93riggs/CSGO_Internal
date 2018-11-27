#pragma once
#include "SourceEngine/QAngle.hpp"
#include "Singleton.h"

class C_CSPlayer;

class Triggerbot : public singleton<Triggerbot>
{
	bool RayHitsEnemy(C_CSPlayer* pLocal) const;
	static bool GetHitGroup(C_CSPlayer* pLocal);

	bool Hitchance(C_CSPlayer* pLocal, QAngle& angle, C_CSPlayer* enemy) const;

public:
	void CreateMove() const;
};

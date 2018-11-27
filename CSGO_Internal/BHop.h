#pragma once
#include "Singleton.h"

class C_CSPlayer;

class BunnyHop : public singleton<BunnyHop>
{
	static void AutoHop(C_CSPlayer* pLocal);
	static void LegitStrafe(C_CSPlayer* pLocal);
	static void RageStrafe(C_CSPlayer* pLocal);
	static void AutoStrafe(C_CSPlayer* pLocal);
	void CircleStrafe(C_CSPlayer* pLocal) const;

public:
	void AirCrouch(C_CSPlayer* pLocal);
	void CreateMove() const;
};

#pragma once
#include "SourceEngine/SDK.hpp"
#include "Singleton.h"

class CBaseCombatWeapon;

class freestanding : public singleton<freestanding>
{
public:
	void edge_anti_aim(CUserCmd*) const;
};

// ReSharper disable once CppInconsistentNaming
typedef int(__stdcall* UTIL_TraceLine_t)(const Vector&, const Vector&, unsigned int, const IClientEntity*, int, trace_t*);
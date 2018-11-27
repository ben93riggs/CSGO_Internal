#pragma once
#include "Singleton.h"

class C_CSPlayer;

class Resolver : public singleton<Resolver>
{
	bool override(C_CSPlayer* pLocal, C_CSPlayer* ent) const;
};

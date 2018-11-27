#pragma once
#include "Singleton.h"

class C_CSPlayer;
class CUserCmd;

class TickFix : public singleton<TickFix>
{
public:
	static int GetTickbase(CUserCmd* ucmd, C_CSPlayer* pLocal);
};


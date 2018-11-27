#include "TickFix.h"
#include "Globals.h"

int TickFix::GetTickbase(CUserCmd* ucmd, C_CSPlayer* pLocal) 
{
	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;

	if (!ucmd || ucmd == g_pLastCmd)
		return g_tick;

	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) 
		g_tick = pLocal->GetTickBase();
	else g_tick++;

	g_pLastCmd = ucmd;

	return g_tick;
}

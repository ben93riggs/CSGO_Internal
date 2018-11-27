#include "DisableInterp.h"
#include "C_CSPlayer.h"
#include "Config.h"

void DisableInterpolation::FrameStageNotify(ClientFrameStage_t stage)
{
	if (stage != FRAME_NET_UPDATE_START)
		return;

	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
		return;

	if (!MenuVars::get().g_bDisableInterp)
		return;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	for (int i = 1; i < 64; i++)
	{
		C_CSPlayer* pEnt = C_CSPlayer::GetPlayerByIndex(i);

		if (!pEnt) continue;
		if (pEnt == pLocal) continue;
		if (pEnt->GetTeam() == pLocal->GetTeam()) continue;
		if (pEnt->IsDormant()) continue;
		if (!pEnt->IsAlive()) continue;
		if (pEnt->GetClientClass()->m_ClassID != CCSPlayer) continue;

		VarMapping_t* map = pEnt->VarMapping();

		if (!map) continue;

		map->m_nInterpolatedEntries = 0;

		//for (int j = 0; j < map->m_nInterpolatedEntries; j++) 
		//{
		//	if (map->m_Entries.IsValidIndex(j))
		//		map->m_Entries[j].m_bNeedsToInterpolate = false;
		//	else
		//		std::cout << map->m_Entries.m_Size << std::endl;
		//}
	}
}

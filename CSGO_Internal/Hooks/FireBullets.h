#pragma once
#include <iostream>
#include "../SourceEngine/CTEFireBullets.h"
#include "../Hooks.hpp"

using TE_FireBullets_PostDataUpdateFn = void(__thiscall*)(C_TEFireBullets*, DataUpdateType_t);
TE_FireBullets_PostDataUpdateFn g_fnOriginalFireBullets_PostDataUpdate;

/*
int m_iPlayer; //0x10
int m_nItemDefIndex; //0x14
Vector m_vecOrigin; //0x18
Vector m_vecAngles; //0x24
int m_weapon; //0x30
int m_iWeaponID; //0x34
int m_iMode; //0x38
int m_iSeed; //0x3C
float m_fInaccuracy; //0x40
float m_flRecoilIndex; //0x44
float m_fSpread; //0x48
int m_iSoundType; //0x4C
 */

inline void __stdcall FireBullets_PostDataUpdate(C_TEFireBullets* ecx, DataUpdateType_t updateType)
{
	//if (!ecx) return g_fnOriginalFireBullets_PostDataUpdate(ecx, updateType);

	//C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();
	//if (!pLocal) return g_fnOriginalFireBullets_PostDataUpdate(ecx, updateType);

	//int iPlayer = ecx->m_iPlayer + 1;
	//if (iPlayer >= 64) return g_fnOriginalFireBullets_PostDataUpdate(ecx, updateType);

	//C_CSPlayer* ent = C_CSPlayer::GetPlayerByIndex(iPlayer);
	//if (!ent) return g_fnOriginalFireBullets_PostDataUpdate(ecx, updateType);
	//if (ent->IsDormant()) return g_fnOriginalFireBullets_PostDataUpdate(ecx, updateType);
	//if (!ent->IsAlive()) return g_fnOriginalFireBullets_PostDataUpdate(ecx, updateType);
	//if (!ent->IsPlayer()) return g_fnOriginalFireBullets_PostDataUpdate(ecx, updateType);
	//if (ent->GetTeam() == pLocal->GetTeam()) return g_fnOriginalFireBullets_PostDataUpdate(ecx, updateType);

	//QAngle calcedAngle = Math::CalcAngle(ent->GetEyePos(), pLocal->GetEyePos());

	//ent->GetEyeAngles().x = calcedAngle.x;
	//ent->GetEyeAngles().y = calcedAngle.y;
	//ent->GetEyeAngles().z = 0.f;

	//float event_time = GLOBALS->tickcount;
	//float shot_time = TICKS_TO_TIME(event_time);

	//for (auto& record : BackTracking::get().m_LagRecord[iPlayer])
	//{
	//	if (record.m_iTickCount <= event_time)
	//	{
	//		shot_time = record.m_flSimulationTime + TICKS_TO_TIME(event_time - record.m_iTickCount); // also get choked from this
	//		break;
	//	}
	//}

	//BackTracking::get().SetOverwriteTick(ent, calcedAngle, shot_time); //somehow create specific record so we can manually restore to this point

	g_fnOriginalFireBullets_PostDataUpdate(ecx, updateType);
}

inline __declspec (naked) void __stdcall TE_FireBullets_PostDataUpdate(DataUpdateType_t updateType)
{
	__asm
	{
		push[esp + 4]
		push ecx
		call FireBullets_PostDataUpdate
		retn 4
	}
}
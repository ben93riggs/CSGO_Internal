#pragma once
#include "Math.hpp"

typedef bool(__thiscall *SetupBones_t)(IClientRenderable*, matrix3x4_t*, int, int, float);
SetupBones_t g_fnOriginalSetupBones;

///https://www.unknowncheats.me/forum/1719493-post23.html
///no idea what g_pPC is and missing a few things. may not be worth to recreate but heres a good reverse of it anyways.
#pragma region NovelaRecreatedSetupBones
//inline void SolveDependencies(DWORD *m_pIk, Vector *pos, Quaternion* q, matrix3x4_t* bonearray, byte *computed)
//{
//	typedef void(__thiscall *pSolveDependencies) (DWORD *m_pIk, DWORD UNKNOWN, Vector *pos, Quaternion* q, matrix3x4_t* bonearray, byte *computed);
//
//	if (oSolveDependencies)
//		oSolveDependencies(m_pIk, 0, pos, q, bonearray, computed);
//}
//
//inline void UpdateTargets(DWORD *m_pIk, Vector *pos, Quaternion* q, matrix3x4_t* bonearray, byte *computed)
//{
//	typedef void(__thiscall *pUpdateTargets) (DWORD *m_pIk, Vector *pos, Quaternion* q, matrix3x4_t* bonearray, byte *computed);
//
//	if (oUpdateTargets)
//		oUpdateTargets(m_pIk, pos, q, bonearray, computed);
//}
//
//inline void ClearTargets(DWORD *m_pIk)
//{
//	//??? valve pls
//	int max = *(int*)((DWORD)m_pIk + 4080);
//
//	int v59 = 0;
//
//	if (max > 0)
//	{
//		DWORD v60 = (DWORD)((DWORD)m_pIk + 208);
//		do
//		{
//			*(int*)(v60) = -9999;
//			v60 += 340;
//			++v59;
//		} while (v59 < max);
//	}
//}
//
//inline void Init(DWORD *m_pIk, studiohdr_t * hdrs, Vector &angles, Vector &pos, float flTime, int iFramecounter, int boneMask)
//{
//	typedef void(__thiscall *pInit) (DWORD *m_pIk, studiohdr_t * hdrs, Vector &angles, Vector &pos, float flTime, int iFramecounter, int boneMask);
//
//	if (oInit)
//		oInit(m_pIk, hdrs, angles, pos, flTime, iFramecounter, boneMask);
//
//}
//
//inline bool SetupBonesOverride(C_CSPlayer* pTarget, matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
//{
//	IClientRenderable* pRenderable = pTarget->GetClientRenderable();
//
//	if (!pRenderable)
//		return false;
//
//	studiohdr_t * hdr = *(studiohdr_t**)((DWORD)pRenderable + 0x2938);
//
//	if (!hdr)
//		return false;
//	
//	//9876 0x2694 boneaccessor
//	//9884 0x269C oldWritableBones  m_WritableBones 
//	//9880 0x2698 oldReadableBones  m_ReadableBones 
//
//	//GetBoneArrayForWrite
//	matrix3x4_t* backup_matrix = *(matrix3x4_t**)((DWORD)pRenderable + 0x2694); //boneaccessor
//
//	if (!backup_matrix)
//		return false;
//
//	int oldReadableBones = *(int*)((DWORD)pRenderable + 0x2698); //m_ReadableBones
//	int bonesMaskNeedRecalc = boneMask | oldReadableBones;
//
//	//raw data
//	Vector origin = g_pPC[pTarget->EntIndex()].LastUnIpOrigin;
//	Vector angles = g_pPC[pTarget->EntIndex()].LastUnIpAngles;
//
//	//back up interpolated data.
//	Vector backup_absorigin = pTarget->GetAbsOrigin();
//	Vector backup_absangles = pTarget->GetAbsAngles();
//
//
//	//backup poses
//	float backup_poses[24];
//
//	//memcpy for noobs
//	for (int ix = 0; ix < 24; ix++)
//		backup_poses[ix] = pTarget->GetPoseParameters()[ix];// pTarget->GetValue<float>(g_pNetVars->m_flPoseParameter + (ix * 4));
//
//	// build matrix using un-interpolated server data.
//	matrix3x4_t parentTransform;
//
//	Math::AngleIMatrix(angles, parentTransform);
//	parentTransform[0][3] = origin.x;
//	parentTransform[1][3] = origin.y;
//	parentTransform[2][3] = origin.z;
//
//	pTarget->SetAbsOrigin(origin);
//	pTarget->SetAbsAngles(angles);
//
//	for (int ix = 0; ix < 24; ix++)
//		pTarget->GetPoseParameters()[ix] = g_pPC[pTarget->EntIndex()].lt_PoseParameter[ix];
//
//	*(int*)((DWORD)pRenderable + 224) |= 8;//AddFlag( EFL_SETTING_UP_BONES ); (1 << 3));
//
//	pTarget->UpdateIKLocks(currentTime);
//
//	DWORD *m_pIk = *(DWORD**)((DWORD)pRenderable + 0x265C);
//
//	if (m_pIk)
//	{
//		ClearTargets(m_pIk);
//		Init(m_pIk, hdr, backup_absangles, backup_absorigin, currentTime, GLOBALS->framecount, bonesMaskNeedRecalc);
//	}
//
//	Vector pos[128];
//	Quaternion q[128];
//	byte computed[256] = { 0 };
//
//	pTarget->StandardBlendingRules(hdr, pos, q, currentTime, bonesMaskNeedRecalc);
//
//	*(matrix3x4_t**)((DWORD)pRenderable + 0x2694) = pBoneToWorldOut; //boneaccessor
//
//	if (m_pIk) 
//	{
//		UpdateTargets(m_pIk, pos, q, *(matrix3x4_t**)((DWORD)pRenderable + 0x2694), computed);
//
//		pTarget->CalculateIKLocks(currentTime);
//
//		SolveDependencies(m_pIk, pos, q, *(matrix3x4_t**)((DWORD)pRenderable + 0x2694), computed);//crash 1
//	}
//
//	pTarget->BuildTransformations(hdr, pos, q, parentTransform, bonesMaskNeedRecalc, computed);//crash 2
//
//	*(int*)((DWORD)pRenderable + 224) &= ~(8);//AddFlag( EFL_SETTING_UP_BONES ); (1 << 3));
//
//	*(matrix3x4_t**)((DWORD)pRenderable + 0x2694) = backup_matrix;
//
//	// restore original entity data.
//	pTarget->SetAbsOrigin(backup_absorigin);
//	pTarget->SetAbsAngles(backup_absangles);
//
//	for (int ix = 0; ix < 24; ix++)
//	{
//		pTarget->GetPoseParameters()[ix] = backup_poses[ix];
//		//pTarget->SetValue<float>(g_pNetVars->m_flPoseParameter + (ix * 4), backup_poses[ix]);
//	}
//
//	return true;
//}
#pragma endregion

inline bool __fastcall hooks::setup_bones(IClientRenderable* ecx, void* edx, matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	if (!ecx)
		return g_fnOriginalSetupBones(ecx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	if (!((IClientRenderable*)ecx)->GetIClientUnknown())
		return g_fnOriginalSetupBones(ecx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	IClientNetworkable* pNetworkable = ((IClientRenderable*)ecx)->GetIClientUnknown()->GetClientNetworkable();

	if (!pNetworkable)
		return g_fnOriginalSetupBones(ecx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	if (!pNetworkable->GetClientClass())
		return g_fnOriginalSetupBones(ecx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	if (pNetworkable->GetClientClass()->m_ClassID != CCSPlayer)
		return g_fnOriginalSetupBones(ecx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	static auto host_timescale = CVAR->FindVar(XorStr("host_timescale"));

	auto player = (C_CSPlayer*)ecx;
	float OldCurTime = GLOBALS->curtime;
	float OldRealTime = GLOBALS->realtime;
	float OldFrameTime = GLOBALS->frametime;
	float OldAbsFrameTime = GLOBALS->absoluteframetime;
	float OldAbsFrameTimeStart = GLOBALS->absoluteframestarttimestddev;
	float OldInterpAmount = GLOBALS->interpolation_amount;
	int OldFrameCount = GLOBALS->framecount;
	int OldTickCount = GLOBALS->tickcount;

	GLOBALS->curtime = player->GetSimulationTime();
	GLOBALS->realtime = player->GetSimulationTime();
	GLOBALS->frametime = GLOBALS->interval_per_tick * host_timescale->GetFloat();
	GLOBALS->absoluteframetime = GLOBALS->interval_per_tick * host_timescale->GetFloat();
	GLOBALS->absoluteframestarttimestddev = player->GetSimulationTime() - GLOBALS->interval_per_tick * host_timescale->GetFloat();
	GLOBALS->interpolation_amount = 0;
	GLOBALS->framecount = TIME_TO_TICKS(player->GetSimulationTime());
	GLOBALS->tickcount = TIME_TO_TICKS(player->GetSimulationTime());

	*(int*)((int)player + 236) |= 8; // IsNoInterpolationFrame
	bool ret_value = g_fnOriginalSetupBones(player, pBoneToWorldOut, nMaxBones, boneMask, GLOBALS->curtime);
	*(int*)((int)player + 236) &= ~8; // (1 << 3)

	GLOBALS->curtime = OldCurTime;
	GLOBALS->realtime = OldRealTime;
	GLOBALS->frametime = OldFrameTime;
	GLOBALS->absoluteframetime = OldAbsFrameTime;
	GLOBALS->absoluteframestarttimestddev = OldAbsFrameTimeStart;
	GLOBALS->interpolation_amount = OldInterpAmount;
	GLOBALS->framecount = OldFrameCount;
	GLOBALS->tickcount = OldTickCount;

	return ret_value;
}
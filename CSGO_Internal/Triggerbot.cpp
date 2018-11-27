#pragma once
#include "Triggerbot.h"
#include "Globals.h"
#include "Config.h"

#include "Utils.hpp"

trace_t tr;

bool Triggerbot::Hitchance(C_CSPlayer* pLocal, QAngle& angle, C_CSPlayer* enemy) const
//second parameter is the angle we want to check against
{
	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_bHitChance)
		return true;

	Vector forward, right, up;

	constexpr auto max_traces = 150;

	Math::AngleVectors(angle, forward, right, up);

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return false;

	const CCSWeaponInfo* weapon_info = weapon->GetCSWpnData();

	if (!weapon_info)
		return false;

	weapon->UpdateAccuracyPenalty();

	float weaponspread = weapon->GetInaccuracy() + weapon->GetSpread();

	if (weaponspread > 1.0f)
		weaponspread = 1.0f;

	weaponspread = (1.0f - weaponspread);

	int total_hits = 0;
	int needed_hits = static_cast<int>((max_traces * 0.9f * weaponspread) * (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_flHitchancePercentage / 100.f));

	auto eyes = pLocal->GetEyePos();
	auto flRange = weapon_info->flRange;

	for (int i = 0; i < max_traces; i++)
	{
		Math::RandomSeed((i & 0xFF) + 1);

		float fRand1 = Math::RandomFloat(0.f, 1.f);
		float fRandPi1 = Math::RandomFloat(0.f, 2.f * (float)M_PI);
		float fRand2 = Math::RandomFloat(0.f, 1.f);
		float fRandPi2 = Math::RandomFloat(0.f, 2.f * (float)M_PI);

		int m_iItemDefinitionIndex = weapon->GetItemDefinitionIndex();
		float m_flRecoilIndex = weapon->GetRecoilIndex();

		if (m_iItemDefinitionIndex == weapon_revolver)
		{
			if (g::cmd->buttons & IN_ATTACK2)
			{
				fRand1 = 1.f - fRand1 * fRand1;
				fRand2 = 1.f - fRand2 * fRand2;
			}
		}
		else if (m_iItemDefinitionIndex == weapon_negev && m_flRecoilIndex < 3.f)
		{
			for (int ii = 3; ii > m_flRecoilIndex; --ii)
			{
				fRand1 *= fRand1;
				fRand2 *= fRand2;
			}

			fRand1 = 1.f - fRand1;
			fRand2 = 1.f - fRand2;
		}

		float fRandInaccuracy = fRand1 * weapon->GetInaccuracy();
		float fRandSpread = fRand2 * weapon->GetSpread();

		float fSpreadX = cos(fRandPi1) * fRandInaccuracy + cos(fRandPi2) * fRandSpread;
		float fSpreadY = sin(fRandPi1) * fRandInaccuracy + sin(fRandPi2) * fRandSpread;

		auto viewSpreadForward = (forward + (right * fSpreadX) + (up * fSpreadY)).Normalized();

		Vector viewAnglesSpread;
		Math::VectorAngles(viewSpreadForward, viewAnglesSpread);
		Math::GUWOPNormalize(viewAnglesSpread);

		Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, &viewForward);

		viewForward = eyes + (viewForward * flRange);

		trace_t tr;
		Ray_t ray;
		ray.Init(eyes, viewForward);

		ENGINETRACE->ClipRayToEntity(ray, MASK_SHOT, enemy, &tr);

		if (tr.m_pEnt == enemy)
			total_hits++;

		if (total_hits >= needed_hits)
			return true;

		if ((max_traces - i + total_hits) < needed_hits)
			return false;
	}

	return false;
}

bool Triggerbot::RayHitsEnemy(C_CSPlayer* pLocal) const
{
	if (!pLocal)
		return false;

	CBaseCombatWeapon* pLocalWeapon = pLocal->GetActiveWeapon();

	if (!pLocalWeapon)
		return false;

	const CCSWeaponInfo* pLocalWeaponInfo = pLocalWeapon->GetCSWpnData();

	if (!pLocalWeaponInfo)
		return false;

	Vector forward;
	Ray_t ray;
	CTraceFilter filter;

	QAngle viewangle = g::cmd->viewangles;

	viewangle += pLocal->AimPunch() * 2.f;

	Math::AngleVectors(viewangle, &forward);

	forward *= pLocalWeaponInfo->flRange;

	filter.pSkip = pLocal;

	Vector src = pLocal->GetEyePos();
	Vector dst = src + forward;

	ray.Init(src, dst);
	ENGINETRACE->TraceRay(ray, MASK_SHOT, &filter, &tr);

	C_CSPlayer* pEntity = (C_CSPlayer*)tr.m_pEnt;

	if (!pEntity)
		return false;

	ClientClass* ent_client_class = pEntity->GetClientClass();

	if (!ent_client_class)
		return false;

	return (pEntity && pEntity->GetTeam() != pLocal->GetTeam() && pEntity->IsAlive() && ent_client_class->m_ClassID == CCSPlayer && !pEntity->GetGunGameImmunity()/* && Hitchance(pLocal, viewangle, pEntity)*/);
}

bool Triggerbot::GetHitGroup(C_CSPlayer* pLocal)
{
	if (!pLocal)
		return false;

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return false;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_bTriggerbotHead && tr.hitgroup == HITGROUP_HEAD)
		return true;
	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_bTriggerbotChest && tr.hitgroup == HITGROUP_CHEST)
		return true;
	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_bTriggerbotStomach && tr.hitgroup == HITGROUP_STOMACH)
		return true;
	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_bTriggerbotArms && (tr.hitgroup == HITGROUP_LEFTARM || tr.hitgroup == HITGROUP_RIGHTARM))
		return true;
	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_bTriggerbotLegs && (tr.hitgroup == HITGROUP_LEFTLEG || tr.hitgroup == HITGROUP_RIGHTLEG))
		return true;

	return false;
}

void Triggerbot::CreateMove() const
{
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_bTriggerbotEnabled && (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_bAutoshoot || INPUTSYSTEM->IsButtonDown(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].menu.g_kTriggerKey)))
	{

		CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

		if (!weapon)
			return;

		bool bDidHit = RayHitsEnemy(pLocal);
		if (bDidHit)
		{
			if (GetHitGroup(pLocal) && weapon->GetWeaponType() != WEAPONTYPE_OTHER && !pLocal->IsInReload())
			{
				if (weapon->GetWeaponType() == WEAPONTYPE_PISTOL)
				{
					if (!pLocal->bCanFire())
					{
						g::cmd->buttons &= ~IN_ATTACK;
					}
					else
					{
						g::cmd->buttons |= IN_ATTACK;
					}
				}
				else 
				{
					g::cmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (!bDidHit && g::aimbotting)
		{
			g::aimbotting = false;
		}
	}
}

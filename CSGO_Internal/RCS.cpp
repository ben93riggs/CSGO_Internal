#include "RCS.h"
#include "Globals.h"


void RCS::LegitRCS(C_CSPlayer* pLocal)
{
	if (g::aimbotting)
		return;

	if (!pLocal)
		return;

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return;

	if (weapon->GetWeaponType() == WEAPONTYPE_PISTOL && !WeaponConfigVars::get().WeaponConfig[g::weaponconfig].misc.g_bAutoPistol)
		return;

	g::cmd->viewangles.x -= (pLocal->AimPunch().x - oldPunchAngles.x) * (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_flRCSPitchScale * 0.02f);
	g::cmd->viewangles.y -= (pLocal->AimPunch().y - oldPunchAngles.y) * (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_flRCSYawScale * 0.02f);
	oldPunchAngles = pLocal->AimPunch();

	Math::ClampMovement(g::cmd);
	Math::Clamp(g::cmd->viewangles);
}

void RCS::SilentRCS(bool& bret, C_CSPlayer* pLocal)
{
	if (g::aimbotting)
		return;

	auto punchAngles = pLocal->AimPunch() * 2;
	auto currentangle = g::cmd->viewangles;
	if (punchAngles.x != 0.0f || punchAngles.y != 0.0f)
	{
		currentangle -= punchAngles;

		g::cmd->viewangles = currentangle;

		bret = false;
	}

	Math::ClampMovement(g::cmd);
	Math::Clamp(g::cmd->viewangles);
	assert(!(g::cmd->viewangles.x >= 90.f || g::cmd->viewangles.x <= -90.f || g::cmd->viewangles.y > 180.f || g::cmd->viewangles.y < -180.f || g::cmd->viewangles.z > .99f));
	
}

void RCS::CreateMove(bool& bret)
{
	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bRCSEnabled)
		return;

	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bStandaloneRCS)
		return;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bSilentRCS)
		SilentRCS(bret, pLocal);
	else
		LegitRCS(pLocal);

	Math::ClampMovement(g::cmd);
	Math::Clamp(g::cmd->viewangles);
	assert(!(g::cmd->viewangles.x >= 90.f || g::cmd->viewangles.x <= -90.f || g::cmd->viewangles.y > 180.f || g::cmd->viewangles.y < -180.f || g::cmd->viewangles.z > .99f));
}

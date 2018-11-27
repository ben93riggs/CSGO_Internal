#include "AutoPistol.h"
#include "Globals.h"

void AutoPistol::CreateMove()
{
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].misc.g_bAutoPistol)
		return;

	if (g::cmd->buttons & IN_ATTACK)
	{
		CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

		if (!weapon)
			return;

		if (weapon->GetWeaponType() == WEAPONTYPE_PISTOL)
		{
			pLocal->bCanFire() ? g::cmd->buttons |= IN_ATTACK : g::cmd->buttons &= ~IN_ATTACK;
		}
	}
}
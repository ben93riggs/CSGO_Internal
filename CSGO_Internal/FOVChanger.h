#pragma once

class FOVChanger : public singleton<FOVChanger>
{
public:
	static void OverrideView(CViewSetup* vsView)
	{
		if (!ENGINE->IsInGame())
			return;

		if (ENGINE->IsTakingScreenshot())
			return;

		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
			return;

		if(!pLocal->IsAlive())
			return;

		CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

		if (!weapon)
			return;

		if (!MiscVars::get().g_bFOVChanger)
			return;

		if (weapon->GetZoomLevel() == 2)
			return;

		if (weapon->GetWeaponType() == WEAPONTYPE_SNIPER && weapon->GetZoomLevel() == 1)
		{
			vsView->fov += 50.f;
			return;
		}
		
		//if (weapon->GetWeaponType() == WEAPONTYPE_OTHER || weapon->GetWeaponType() == WEAPONTYPE_SHOTGUN)
		//	return;

		vsView->fov += (float)(MiscVars::get().g_iFOVChangerValue - 90);
	}
};
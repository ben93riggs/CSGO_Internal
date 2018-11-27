#pragma once

class WeaponConfigs : public singleton<WeaponConfigs>
{
	static int GetWeaponConfig(CBaseCombatWeapon* weapon)
	{
		//if weapon is a c4 or knife or grende don't load config
		if (weapon->GetWeaponType() == WEAPONTYPE_OTHER)
			return weapon_none;

		EItemDefinitionIndex weaponid = (EItemDefinitionIndex)weapon->GetId();

		if (WeaponConfigVars::get().WeaponConfig[(int)weaponid].bEnabled)
			return (weaponid);

		return weapon_none;
	}

	static int GetWeaponID(CBaseCombatWeapon* weapon)
	{
		//if weapon is a c4 or knife or grende don't load config
		if (weapon->GetWeaponType() == WEAPONTYPE_OTHER)
			return weapon_none;

		EItemDefinitionIndex weaponid = (EItemDefinitionIndex)weapon->GetId();
		return (weaponid);
	}

public:
	static void FrameStageNotify(ClientFrameStage_t stage)
	{
		if (stage != FRAME_NET_UPDATE_END)
			return;

		if (MenuVars::get().g_bWeaponConfigWindow && MenuVars::get().g_bMainWindow)
			return;

		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
		{
			g::currentweaponid = 0;
			g::weaponconfig = 0;
			return;
		}

		if (pLocal->IsAlive())
		{

			CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

			if (!weapon)
			{
				g::currentweaponid = 0;
				g::weaponconfig = 0;
				return;
			}

			g::currentweaponid = GetWeaponID(weapon);
			g::weaponconfig = GetWeaponConfig(weapon);
		}
	}
};

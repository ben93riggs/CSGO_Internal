#pragma once
#include "Legitbot.h"
#include "Ragebot.h"
#include "NoSpreadBot.h"

class Aimbot : public singleton<Aimbot>
{
	enum
	{
		AIM_MODE_LEGIT,
		AIM_MODE_SPREAD
	};

public:
	static void CreateMove(bool& bRet, bool& bSendPacket)
	{
		if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bAimbotEnabled)
		{ 
			g::aimbotting = false;
			return;
		}

		if (MenuVars::get().g_iCurrentAimbotMode == AIM_MODE_LEGIT)
			Legitbot::get().CreateMove(bRet, bSendPacket);
		else if (MenuVars::get().g_iCurrentAimbotMode == AIM_MODE_SPREAD)
			RageBot::get().CreateMove(bRet, bSendPacket);

		Math::Clamp(g::cmd->viewangles);
		Math::ClampMovement(g::cmd);
	}
};

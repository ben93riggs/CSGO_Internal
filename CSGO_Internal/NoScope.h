#pragma once

class NoScope : public singleton<NoScope>
{
public:
	static void FrameStageNotify(ClientFrameStage_t stage)
	{
		if (stage != FRAME_RENDER_START)
			return;

		if (!VisualVars::get().Removals.g_bNoScope)
			return;

		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
			return;

		CBaseCombatWeapon* localweap = pLocal->GetActiveWeapon();

		if (!localweap)
			return;

		if (localweap->GetWeaponType() != WEAPONTYPE_SNIPER)
			return;

		if (localweap->GetZoomLevel() == 0)
			return;

		pLocal->IsScoped() = false;
	}
};
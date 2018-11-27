#pragma once

class NoVisualRecoil : public singleton<NoVisualRecoil>
{
	QAngle aim_punch_old;
	QAngle view_punch_old;
	QAngle* aim_punch = nullptr;
	QAngle* view_punch = nullptr;
	
public:
	void FrameStageNotify_pre(ClientFrameStage_t stage)
	{
		aim_punch_old = Vector();
		view_punch_old = Vector();
		aim_punch = nullptr;
		view_punch = nullptr;

		if (stage != FRAME_RENDER_START)
			return;

		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
			return;

		if (!pLocal->IsAlive())
			return;

		//no visual recoil part 1
		if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bSilentRCS)
			return;

		aim_punch = &pLocal->AimPunch();
		view_punch = &pLocal->ViewPunch();

		aim_punch_old = *aim_punch;
		view_punch_old = *view_punch;

		*aim_punch = QAngle(0, 0, 0);
		*view_punch = QAngle(0, 0, 0);
	}

	void FrameStageNotify_post(ClientFrameStage_t stage) const
	{
		if (stage != FRAME_RENDER_START)
			return;
		
		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
			return;

		if (!pLocal->IsAlive())
			return;

		//no visual recoil part 1
		if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bSilentRCS)
			return;

		if (!aim_punch)
			return;

		if (!view_punch)
			return;

		*aim_punch = aim_punch_old;
		*view_punch = view_punch_old;
	}

};
#pragma once

class ThirdPersonAA : public singleton<ThirdPersonAA>
{
	Vector o_m_vecAngles;

public:
	void FrameStageNotify_pre(ClientFrameStage_t stage)
	{
		if (stage != FRAME_RENDER_START)
			return;

		if (!AAVars::get().g_bAAEnabled)
			return;

		if (!MiscVars::get().g_bThirdPerson)
			return;

		if (!INPUT->m_fCameraInThirdPerson)
			return;

		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
			return;
		
		if (!pLocal->IsAlive())
			return;

		o_m_vecAngles = pLocal->vecAngles(); //back up angles

		pLocal->vecAngles() = g::anti_aim_angles;
		
	}

	void FrameStageNotify_post(ClientFrameStage_t stage) const
	{
		if (stage != FRAME_RENDER_START)
			return;

		if (!AAVars::get().g_bAAEnabled)
			return;

		if (!MiscVars::get().g_bThirdPerson)
			return;

		if (!INPUT->m_fCameraInThirdPerson)
			return;
		
		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
			return;

		if (!pLocal->IsAlive())
			return;

		pLocal->vecAngles() = o_m_vecAngles;
	}
};
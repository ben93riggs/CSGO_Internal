#pragma once

class NoFlash : public singleton<NoFlash>
{
public:
	static void FrameStageNotify(ClientFrameStage_t stage)
	{
		if (stage != FRAME_RENDER_START)
			return;

		if (!VisualVars::get().Removals.g_bNoFlash)
			return;

		if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
			return;

		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
			return;

		pLocal->GetFlashDuration() = 0.f;
	}
};
#include "PunchAngleFix.h"
#include "Globals.h"
#include "TickFix.h"

void PunchAngleFix::PunchAngleFix_RunCommand(C_CSPlayer* base_player)
{
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (!pLocal->IsAlive())
		return;
	
	if (pLocal == base_player)
		m_aimPunchAngle[TickFix::get().GetTickbase(g::cmd, pLocal) % 128] = pLocal->AimPunch();
}

void PunchAngleFix::PunchAngleFix_FSN(ClientFrameStage_t stage)
{
	if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		return;

	if (!g::cmd)
		return;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (!pLocal->IsAlive())
		return;

	QAngle new_punch_angle = m_aimPunchAngle[TickFix::get().GetTickbase(g::cmd, pLocal) % 128];

	if (!new_punch_angle.IsValid())
		return;

	pLocal->AimPunch() = new_punch_angle;
	
}
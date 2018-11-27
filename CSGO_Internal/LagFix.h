#pragma once

class LagFix : public Singleton<LagFix>
{
	float OldSimulationTimes[64];

public:
	static void FrameStageNotify(ClientFrameStage_t stage)
	{
		if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START)
			return;

		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
			return;

		if (!pLocal->IsAlive())
			return;
		
		for (int i = 0; i < 64; i++)
		{
			C_CSPlayer* pEnt = static_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));

			if (!pEnt)
			{
				G::ChokedPackets[i] = 0;
				continue;
			}
			
			if (pEnt == pLocal)
				continue;

			if (pEnt->IsDormant() || !pEnt->IsAlive() || !pEnt->IsPlayer())
			{
				G::ChokedPackets[i] = 0;
				continue;
			}

			G::ChokedPackets[i] = pEnt->GetNumberOfTicksChoked();
		}
	}
};
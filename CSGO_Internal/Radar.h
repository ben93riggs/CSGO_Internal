#pragma once

class Radar : public singleton<Radar>
{
public:
	static void FrameStageNotify(ClientFrameStage_t stage)
	{
		if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			if (!VisualVars::get().g_bRadar)
				return;

			C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

			if (!pLocal)
				return;

			for (int i = 0; i < 64; i++)
			{
				C_CSPlayer* ent = (C_CSPlayer*)ENTITYLIST->GetClientEntity(i);

				if (!ent)
					continue;

				if (ent->IsDormant())
					continue;

				if (!ent->IsAlive())
					continue;

				if (ent->GetTeam() == pLocal->GetTeam())
					continue;

				ClientClass* client_class = ent->GetClientClass();

				if (!client_class)
					continue;

				if (client_class->m_ClassID != CCSPlayer)
					continue;

				ent->GetSpotted() = true;
			}
		}
	}
};
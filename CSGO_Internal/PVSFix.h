#pragma once

class PVSFix : public singleton<PVSFix>
{
public:
	static void FrameStageNotify(ClientFrameStage_t curStage)
	{
		if (curStage != FRAME_RENDER_START)
			return;

		for (int i = 0; i <= GLOBALS->maxClients; i++)
		{
			if (i == ENGINE->GetLocalPlayer()) continue;

			IClientEntity* pCurEntity = ENTITYLIST->GetClientEntity(i);
			if (!pCurEntity) continue;

			*(int*)((uintptr_t)pCurEntity + 0xA30) = GLOBALS->framecount; //we'll skip occlusion checks now
			*(int*)((uintptr_t)pCurEntity + 0xA28) = 0;//clear occlusion flags
		}
	}
};
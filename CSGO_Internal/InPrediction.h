#pragma once
typedef bool(__thiscall* InPrediction_t)(void*);
InPrediction_t g_fnOriginalInPrediction;

inline bool __fastcall hooks::in_prediction(void* ecx, void* edx)
{
	static bool data_open[64];
	static auto last_bone_setup_time = GET_NETVAR(XorStr("DT_BaseAnimating"), XorStr("m_hLightingOrigin")) - 0x20;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return g_fnOriginalInPrediction(ecx);

	if (*reinterpret_cast<uint32_t*>(_ReturnAddress()) == 0x875c084) //no idea what this is. i should find out.
	{
		for (auto i = 1; i < GLOBALS->maxClients; i++)
		{
			auto entity = (C_CSPlayer*)ENTITYLIST->GetClientEntity(i);

			if (!entity)
				continue;
			if (entity->IsDormant())
				continue;
			if (!entity->IsAlive())
				continue;
			if (pLocal->GetTeam() == entity->GetTeam())
				continue;
			if (entity->GetGunGameImmunity())
				continue;

			// Force setupbones to not be called
			if (pLocal->GetActiveWeapon() != nullptr)
			{
				data_open[i] = true;
				*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(entity) + last_bone_setup_time) = sqrt(-1.f);
			}
			else if (data_open[i] == true)
			{
				data_open[i] = false;
				*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(entity) + last_bone_setup_time) = GLOBALS->curtime;
			}
		}
	}

	return g_fnOriginalInPrediction(ecx);
}
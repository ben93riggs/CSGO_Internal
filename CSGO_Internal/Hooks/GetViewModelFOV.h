#pragma once

typedef float(__thiscall *GetViewModelFOV_t)(void*);
GetViewModelFOV_t g_fnOriginalGetViewModelFOV;

inline float __fastcall hooks::get_view_model_fov(void* ecx, void* edx)
{
	if (!MenuVars::get().g_bGetViewModelFOV)
		return  g_fnOriginalGetViewModelFOV(ecx);

	if (ENGINE->IsTakingScreenshot() || !ENGINE->IsInGame() || !ENGINE->IsConnected())
		return g_fnOriginalGetViewModelFOV(ecx);

	if (MiscVars::get().g_bThirdPerson)
		return g_fnOriginalGetViewModelFOV(ecx);

	if (MiscVars::get().g_bViewModelFOVChanger)
		return MiscVars::get().g_iViewModelFOVChangerValue;

	return g_fnOriginalGetViewModelFOV(ecx);
}
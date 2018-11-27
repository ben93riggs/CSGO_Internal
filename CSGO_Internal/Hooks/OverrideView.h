#pragma once
#include "ThirdPerson.h"
#include "../FOVChanger.h"


typedef void(__thiscall* OverrideView_t)(IClientMode* ecx, CViewSetup* vsView);
OverrideView_t g_fnOriginalOverrideView;

inline void __fastcall hooks::override_view(IClientMode* ecx, void* edx, CViewSetup* vs_view)
{
	if (!MenuVars::get().g_bOverrideView)
		return g_fnOriginalOverrideView(ecx, vs_view);

	if (ENGINE->IsTakingScreenshot() || !ENGINE->IsInGame() || !ENGINE->IsConnected())
		return g_fnOriginalOverrideView(ecx, vs_view);

	if (!vs_view)
		return g_fnOriginalOverrideView(ecx, vs_view);

	C_CSPlayer * pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return g_fnOriginalOverrideView(ecx, vs_view);

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return g_fnOriginalOverrideView(ecx, vs_view);

	ThirdPerson::get().OverrideView(vs_view);
	FOVChanger::get().OverrideView(vs_view);

	g::current_fov = vs_view->fov;

	CCSGrenadeHint::get().View();
	g_fnOriginalOverrideView(ecx, vs_view);
}
#pragma once
#include "../Chams.h"

typedef void(__thiscall* DrawModelExecute_t)(IVModelRender*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);
DrawModelExecute_t g_fnOriginalDrawModelExecute;

constexpr float reset_val[] = { 1.f, 1.f, 1.f };

inline void __fastcall hooks::draw_model_execute(IVModelRender* ecx, void* edx, void* context, void* state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld)
{
	if (!MenuVars::get().g_bDrawModelExecute)
		return g_fnOriginalDrawModelExecute(ecx, context, state, info, pCustomBoneToWorld);;

	if (ENGINE->IsTakingScreenshot() || !ENGINE->IsInGame() || !ENGINE->IsConnected())
		return g_fnOriginalDrawModelExecute(ecx, context, state, info, pCustomBoneToWorld);

	bool dont_call_original = false;
	Chams::get().draw_model_execute(g_fnOriginalDrawModelExecute, ecx, context, state, info, pCustomBoneToWorld, dont_call_original);

	if (dont_call_original)
		return;

	g_fnOriginalDrawModelExecute(ecx, context, state, info, pCustomBoneToWorld);

	Chams::get().mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
	Chams::get().mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	Chams::get().mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
	Chams::get().mat->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);

	RENDERVIEW->SetColorModulation(reset_val);
	MODELRENDER->ForcedMaterialOverride(nullptr);
}

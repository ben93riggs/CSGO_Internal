#pragma once

typedef void(__thiscall* scene_end_t)(IVRenderView*);
scene_end_t o_scene_end;

inline void __fastcall hooks::scene_end(IVRenderView* ecx, void* edx)
{
	if (!MenuVars::get().g_bSceneEnd)
		return;

	Chams::scene_end();

	o_scene_end(ecx);
}
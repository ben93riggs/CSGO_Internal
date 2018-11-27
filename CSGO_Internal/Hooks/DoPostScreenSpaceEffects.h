#pragma once
#include "../Glow.h"

typedef int(__thiscall* do_post_screen_space_effects_t)(IClientMode*, int);
do_post_screen_space_effects_t o_do_post_screen_space_effects;

inline int __fastcall hooks::do_post_screen_space_effects(IClientMode* ecx, void* edx, int a1)
{
	if (!MenuVars::get().g_bDoPostScreenSpaceEffects)
		return o_do_post_screen_space_effects(ecx, a1);

	Glow::get().DoPostScreenSpaceEffects();

	return o_do_post_screen_space_effects(ecx, a1);
}
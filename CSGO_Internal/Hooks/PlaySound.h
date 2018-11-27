#pragma once
#include <Windows.h>
#include "../XorStr.hpp"
#include "../AutoAccept.h"


typedef void(__thiscall *PlaySound_t)(void*, const char*);
PlaySound_t g_fnOriginalPlaySound;

inline void __fastcall hooks::play_sound(void* ecx, void* edx, const char* filename)
{
	g_fnOriginalPlaySound(ecx, filename);

	if (!MenuVars::get().g_bPlaySound)
		return;

	AutoAccept::PlaySound(filename);
}
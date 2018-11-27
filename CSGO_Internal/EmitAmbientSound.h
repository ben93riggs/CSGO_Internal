#pragma once
#include "SourceEngine/IEngineSound.h"

using EmitAmbientSound_t = void(__thiscall*)(IEngineSound*, const char *pSample, float flVolume, int iPitch, int flags, float soundtime);
EmitAmbientSound_t EmitAmbientSound;

inline void __fastcall Hooked_EmitAmbientSound(IEngineSound* ecx, void* edx, const char* pSample, float flVolume, int iPitch, int flags, float soundtime)
{
	
}
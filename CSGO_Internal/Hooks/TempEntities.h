#pragma once

typedef bool(__thiscall* TempEntities_t)(void*, void*);
TempEntities_t g_fnOriginalTempEntities;

inline bool __fastcall hooks::temp_entities(void* ecx, void* edx, void* msg)
{
	return g_fnOriginalTempEntities(ecx, msg);
}

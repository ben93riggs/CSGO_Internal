#pragma once
#include "KeyState.h"

using SetKeyCodeState_t = void(__thiscall*)(IInputInternal*, ButtonCode_t, bool);
SetKeyCodeState_t g_fnOriginalSetKeyCodeState;

inline void __fastcall Hooks::Hooked_SetKeyCodeState(IInputInternal* ecx, void* edx, ButtonCode_t code, bool bPressed)
{
	//if (SetKeyCodeState::shouldListen && bPressed)
	//{
	//	SetKeyCodeState::shouldListen = false;
	//	*SetKeyCodeState::keyOutput = code;
	//}

	return g_fnOriginalSetKeyCodeState(ecx, code, bPressed);
}

#pragma once

using SetMouseCodeState_t = void(__thiscall*)(IInputInternal*, ButtonCode_t, MouseCodeState_t);
SetMouseCodeState_t g_fnOriginalSetMouseCodeState;

inline void __fastcall Hooks::Hooked_SetMouseCodeState(IInputInternal* ecx, void* edx, ButtonCode_t code, MouseCodeState_t state)
{
	/*if (SetKeyCodeState::shouldListen && state == MouseCodeState_t::BUTTON_PRESSED)
	{
		SetKeyCodeState::shouldListen = false;
		*SetKeyCodeState::keyOutput = code;
	}*/

	if (state == MouseCodeState_t::BUTTON_PRESSED)
		std::cout << Interface->InputSystem()->ButtonCodeToString(code) << std::endl;

	return g_fnOriginalSetMouseCodeState(ecx, code, state);
}
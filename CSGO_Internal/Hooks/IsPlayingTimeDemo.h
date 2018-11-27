#pragma once
//bool                  IsPlayingTimeDemo(void) = 0;
//index 84

typedef bool(__thiscall* IsPlayingTimeDemo_t)(IVEngineClient*);
IsPlayingTimeDemo_t g_fnOriginalIsPlayingTimeDemo;

inline bool __fastcall hooks::is_playing_time_demo(IVEngineClient* ecx, void* edx)
{
	//return g_fnOriginalIsPlayingTimeDemo(ecx);

	if (MenuVars::get().g_iCurrentAimbotMode == 0 || !MenuVars::get().g_bDisableInterp)
		return g_fnOriginalIsPlayingTimeDemo(ecx);

	//ida paste to make sure people think I know what im doing
	//.text:101E2957		mov     s_bInterpolate, bl
	//.text:101E295D		mov     eax, [ecx]
	//.text:101E295F		mov     eax, [eax+150h] ; IsPlayingTimeDemo (engine, 84)
	//.text:101E2965		call    eax
	//.text:101E2967		test    al, al

	static uintptr_t ret = 0;
	if (!ret)
		ret = Utils::FindPattern(Utils::get_client_dll(), XorStr("84 C0 75 14 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0 74 07"));


	if (_ReturnAddress() == (void*)ret)
		return true; // wow! disabled interpolation!!

	return g_fnOriginalIsPlayingTimeDemo(ecx);
}
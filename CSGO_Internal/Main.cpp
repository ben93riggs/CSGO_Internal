#include "Hooks.hpp"
#include "Globals.h"

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)initialize, nullptr, NULL, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		restore();
		break;
	default: break;
	}

	return TRUE;
	 
}
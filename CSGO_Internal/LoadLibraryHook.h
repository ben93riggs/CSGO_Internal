#pragma once
#include <windows.h>

namespace LoadLibraryHook
{
	static DWORD numberOfSafeMods = 1; //number of safe modules (must match the SafeModules array)
	static LPCSTR SafeModules[] = { ""/*, "d3d9.dll"*/ }; //your safe modules, woeful protection, but it's the building block...could replace this list with SHA256 hashes or w/e.

	static DWORD *CurrentPtr; //the LoadLibrary pointer.
	static DWORD LoadLibraryAddress; //the value that the LoadLibrary pointer is SUPPOSED to point to :P

	void SetPointer(DWORD *Address, DWORD *Hook);
	void SetHook();
	bool in_array(LPCSTR haystack[], LPCSTR needle, int sz);
	void __stdcall Hooked_LoadLibrary(LPCSTR hModule);
}


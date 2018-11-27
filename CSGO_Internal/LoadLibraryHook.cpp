#include "LoadLibraryHook.h"
#include <iostream>

DWORD numberOfSafeMods = 1; //number of safe modules (must match the SafeModules array)
LPCSTR SafeModules[] = { ""/*, "d3d9.dll"*/ }; //your safe modules, woeful protection, but it's the building block...could replace this list with SHA256 hashes or w/e.

DWORD *CurrentPtr; //the LoadLibrary pointer.
DWORD LoadLibraryAddress; //the value that the LoadLibrary pointer is SUPPOSED to point to :P

void LoadLibraryHook::SetPointer(DWORD *Address, DWORD *Hook)
{
	*Address = (DWORD)Hook; //set the value that Address points to point at Hook.
}

void LoadLibraryHook::SetHook()
{
	_asm
	{
		lea eax, LoadLibrary;
		mov CurrentPtr, eax;
	}
}

bool LoadLibraryHook::in_array(LPCSTR haystack[], LPCSTR needle, int sz)
{
	//sz is the number of elements in the haystack array.
	//check if the needle is in the haystack, straightforward sequential searching.
	for (int i = 0; i < sz; i++)
	{
		if (strcmp(haystack[i], needle) == 0) { return true; }
	}
	return false; //if we made it here without returning true, we couldn't find it so return false.
}

/** METHODS **/

void __stdcall LoadLibraryHook::Hooked_LoadLibrary(LPCSTR hModule)
{
	std::cout << hModule << std::endl;

	//in this case I just compared file names (not paths), it's way too easy to detour this if you knew that it
	//only checked names, because you can have multiple files with the same names. A better way would be to 
	//create a list of accepted MD5s /SHA1's, but cbf figuring out how to calculate an MD5 in C++.
	std::string rawName = std::string(hModule);
	rawName = rawName.substr(rawName.find_last_of("\\") + 1);
	LPCSTR Filename = (const char*)rawName.c_str();

	if (in_array(LoadLibraryHook::SafeModules, Filename, LoadLibraryHook::numberOfSafeMods)) //if it's a safe module..
	{
		//unhook
		SetPointer(LoadLibraryHook::CurrentPtr, (DWORD*)LoadLibraryHook::LoadLibraryAddress);
		//call original func
		LoadLibraryA(hModule); //call original LoadLibrary
		//rehook
		SetPointer(LoadLibraryHook::CurrentPtr, (DWORD*)&Hooked_LoadLibrary);
	}
}
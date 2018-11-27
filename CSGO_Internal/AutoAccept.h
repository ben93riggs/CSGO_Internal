#pragma once

#undef PlaySound
namespace AutoAccept
{
	using IsReadyFn = void(__cdecl*)();
	IsReadyFn IsReady;
	inline void PlaySound(const char* filename)
	{
		if (ENGINE->IsInGame() || !MiscVars::get().g_bAutoAccept)
			return;

		//This is the beep sound that is played when we have found a game
		if (!strcmp(filename, XorStr("!UI/competitive_accept_beep.wav")))
		{
			//This is the function that is called when you press the big ACCEPT button
			if (!IsReady)
				IsReady = (IsReadyFn)(Utils::FindSignature(Utils::get_client_dll(), XorStr("56 8B 35 ? ? ? ? 57 83 BE")) - 9);

			//Accept the game
			IsReady();
			
			//This will flash the CSGO window on the taskbar
			//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
			FLASHWINFO fi;
			fi.cbSize = sizeof(FLASHWINFO);
			fi.hwnd = g::window;
			fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
			fi.uCount = 0;
			fi.dwTimeout = 0;
			FlashWindowEx(&fi);
		}
	}
}
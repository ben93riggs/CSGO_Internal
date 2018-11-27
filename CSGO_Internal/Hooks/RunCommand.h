#pragma once

typedef void(__thiscall* RunCommand_t)(void*, C_CSPlayer*, CUserCmd*, IMoveHelper*);
RunCommand_t g_fnOriginalRunCommand;

inline void __fastcall Hooks::Hooked_RunCommand(void* thisptr, void* edx, C_CSPlayer *player, CUserCmd *pCmd, IMoveHelper *moveHelper)
{
	g_fnOriginalRunCommand(thisptr, player, pCmd, moveHelper);

	//PunchAngleFix::get().PunchAngleFix_RunCommand(player);
}
#include "RankRevealer.h"
#include "Globals.h"

void RankRevealer::CreateMove() const
{
	if (!MiscVars::get().g_bShowRanks)
		return;
	if (!INPUTSYSTEM->IsButtonDown(static_cast<ButtonCode_t>(KEY_TAB)))
		return;
	return;

	static float fArray[3] = { 0.f, 0.f, 0.f };

	typedef bool(__cdecl* ServerRankRevealAllFn)(float*);
	static ServerRankRevealAllFn ServerRankRevealAllEx = (ServerRankRevealAllFn)(Utils::FindSignature(Utils::get_client_dll(), Utils::is_panorama() ? XorStr("55 8B EC 8B 0D ? ? ? ? 85 C9 75 ? A1 ? ? ? ? 68 ? ? ? ? 8B 08 8B 01 FF 50 ? 85 C0 74 ? 8B C8 E8 ? ? ? ? 8B C8 EB ? 33 C9 89 0D ? ? ? ? 8B 45 ? FF 70 ? E8 ? ? ? ? B0 ? 5D") : XorStr("55 8B EC 8B 0D ? ? ? ? 68")));

	if (!ServerRankRevealAllEx)
		return;

	ServerRankRevealAllEx(fArray);
}

#include "NoSmoke.h"
#include "Globals.h"

void NoSmoke::FrameStageNotify(ClientFrameStage_t stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (!VisualVars::get().Removals.g_bNoSmoke)
		return;

	if (!smoke_count_offset)
		smoke_count_offset = *(DWORD*)(Utils::FindPattern(Utils::get_client_dll(), XorStr("A3 ?? ?? ?? ?? 57 8B CB")) + 0x1);

	*(int*)(smoke_count_offset) = 0;
}
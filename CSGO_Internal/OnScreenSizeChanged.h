#pragma once
#include "SourceEngine/SDK.hpp"
#include "CRender.h"

typedef void(__thiscall* OnScreenSizeChanged_t)(ISurface*, int, int);
OnScreenSizeChanged_t g_fnOriginalOnScreenSizeChanged;

inline void __fastcall hooks::on_screen_size_changed(ISurface* ecx, void* edx, int oldWidth, int oldHeight)
{
	g_fnOriginalOnScreenSizeChanged(Interface->MatSurface(), oldWidth, oldHeight);

	CRender::get().SetupFonts();
}

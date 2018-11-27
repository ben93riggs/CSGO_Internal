#include "WorldModifications.h"
#include "Globals.h"

void WorldModifications::Greymode()
{
	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
		return;

	static ConVar* greyConvar = CVAR->FindVar(XorStr("mat_drawgray"));
	*reinterpret_cast<int*>(reinterpret_cast<DWORD>(&greyConvar->m_fnChangeCallbacks) + 0xC) = NULL;

	if (!VisualVars::get().g_bGreyMode)
	{
		greyConvar->SetValue(XorStr("0"));
		return;
	}

	greyConvar->SetValue(XorStr("1"));
}

void WorldModifications::Fullbright()
{
	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
		return;

	static ConVar* fullbrightConvar = CVAR->FindVar(XorStr("mat_fullbright"));
	*reinterpret_cast<int*>(reinterpret_cast<DWORD>(&fullbrightConvar->m_fnChangeCallbacks) + 0xC) = NULL;

	if (!VisualVars::get().g_bFullbright)
	{
		fullbrightConvar->SetValue(XorStr("0"));
		return;
	}

	fullbrightConvar->SetValue(XorStr("1"));
}

void WorldModifications::Psychedelic()
{
	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
		return;

	static ConVar* trippyConvar = CVAR->FindVar(XorStr("mat_showmiplevels"));
	*reinterpret_cast<int*>(reinterpret_cast<DWORD>(&trippyConvar->m_fnChangeCallbacks) + 0xC) = NULL;

	if (!VisualVars::get().g_bPsychedelic)
	{
		trippyConvar->SetValue(XorStr("0"));
		return;
	}

	trippyConvar->SetValue(XorStr("3"));
}

void WorldModifications::Pixelated()
{
	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
		return;

	// mat_picmip "2"
	static ConVar* pixelConvar = CVAR->FindVar(XorStr("mat_showlowresimage"));
	*reinterpret_cast<int*>(reinterpret_cast<DWORD>(&pixelConvar->m_fnChangeCallbacks) + 0xC) = NULL;

	static ConVar* picmipConvar = CVAR->FindVar(XorStr("mat_picmip"));
	*reinterpret_cast<int*>(reinterpret_cast<DWORD>(&picmipConvar->m_fnChangeCallbacks) + 0xC) = NULL;

	if (!VisualVars::get().g_bPixelated)
	{
		picmipConvar->SetValue(XorStr("0"));
		pixelConvar->SetValue(XorStr("0"));
		return;
	}

	picmipConvar->SetValue(XorStr("2"));
	pixelConvar->SetValue(XorStr("1"));
}

void WorldModifications::PostProcessing()
{
	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
		return;

	static ConVar* postprocessConvar = CVAR->FindVar(XorStr("mat_postprocess_enable"));
	*reinterpret_cast<int*>(reinterpret_cast<DWORD>(&postprocessConvar->m_fnChangeCallbacks) + 0xC) = NULL;

	if (!VisualVars::get().g_bPostProcess)
	{
		postprocessConvar->SetValue(XorStr("1"));
		return;
	}

	postprocessConvar->SetValue(XorStr("0"));
}

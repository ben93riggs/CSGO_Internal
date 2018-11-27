#include "CustomSky.h"
#include "Globals.h"

void CustomSky::Update() const
{
	static ConVar* sv_skyname = CVAR->FindVar(XorStr("sv_skyname"));

	if (!sv_skyname)
		return;

	sv_skyname->m_nFlags &= ~FCVAR_CHEAT;
	sv_skyname->SetValue(skylist[VisualVars::get().g_iCustomSky]);
}

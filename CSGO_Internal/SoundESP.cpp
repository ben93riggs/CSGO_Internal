#include "SoundESP.h"
#include "Globals.h"

void SoundESP::EmitSound(int iEntIndex, const Vector *pOrigin) const
{
	if (!VisualVars::get().ESP.g_bFootstepESP)
		return;

	if (iEntIndex > 64 || iEntIndex <= 1)
		return;

	C_CSPlayer* pEnt = (C_CSPlayer*)ENTITYLIST->GetClientEntity(ENGINE->GetPlayerForUserID(iEntIndex));

	if (!pEnt)
		return;

	ClientClass* ent_client_class = pEnt->GetClientClass();

	if (!ent_client_class)
		return;

	if (ent_client_class->m_ClassID != CCSPlayer)
		return;

	//if (pLocal->GetTeam() != pEnt->GetTeam())
	//	return;

	if (!pEnt->IsAlive())
		return;

	if (!pOrigin)
		return;

	Vector temp = *pOrigin;

	if (temp.IsValid())
		DEBUGOVERLAY->AddBoxOverlay2(temp, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), filler, border, 1.f);
}

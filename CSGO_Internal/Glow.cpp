#include "Glow.h"
#include "Vars.h"
#include "Utils.hpp"

void Glow::DoPostScreenSpaceEffects()
{
	if (!VisualVars::get().Glow.g_bGlowEnabled)
		return;

	if (!ENGINE->IsConnected() || !ENGINE->IsInGame())
		return;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (!GlowObjectManager)
		GlowObjectManager = reinterpret_cast<CGlowObjectManager*(__cdecl*)()>(Utils::FindSignature(Utils::get_client_dll(), XorStr("A1 ? ? ? ? A8 01 75 4B")))();

	if (!GlowObjectManager)
		return;

	for (int i = 0; i < GlowObjectManager->size; ++i)
	{
		GlowObjectDefinition_t* glowEntity = &GlowObjectManager->m_GlowObjectDefinitions[i];

		if (!glowEntity)
			continue;

		if (glowEntity->IsEmpty())
			continue;

		if (!glowEntity->getEntity())
			continue;

		C_CSPlayer* Entity = static_cast<C_CSPlayer*>(glowEntity->getEntity());

		if (!Entity)
			continue;

		ClientClass* ent_client_class = Entity->GetClientClass();

		if (!ent_client_class)
			continue;

		switch (ent_client_class->m_ClassID)
		{
		case CSCAR17:
		case CDEagle:
		case CAK47:
			if (VisualVars::get().Glow.g_bWeapons)
				glowEntity->set(weaponColor, VisualVars::get().Glow.g_flGlowScale);
			break;

		case CBaseCSGrenadeProjectile:
		case CDecoyProjectile:
		case CMolotovProjectile:
		case CSensorGrenadeProjectile:
		case CSmokeGrenadeProjectile:
			if (VisualVars::get().Glow.g_bProjectiles)
				glowEntity->set(grenadeColor, VisualVars::get().Glow.g_flGlowScale);
			break;

		case CPlantedC4:
		case CC4:
			if (VisualVars::get().Glow.g_bC4)
				glowEntity->set(c4Color, VisualVars::get().Glow.g_flGlowScale);
			break;

		case CChicken:
			if (VisualVars::get().Glow.g_bChicken)
				glowEntity->set(chickenColor, VisualVars::get().Glow.g_flGlowScale);
			break;

		case CCSPlayer:
			if (VisualVars::get().Glow.g_bGlowEnabled)
			{
				if (!VisualVars::get().Glow.g_bTeammates && Entity->GetTeam() == pLocal->GetTeam())
					break;

				if (!VisualVars::get().Glow.g_bEnemies && Entity->GetTeam() != pLocal->GetTeam())
					break;

				if (VisualVars::get().Glow.g_bVisCheck)
					if (!Entity->IsVisible(Utils::GetHitboxPosition(Entity, HITBOX_HEAD)) && !Entity->IsVisible(Utils::GetHitboxPosition(Entity, HITBOX_LEFT_HAND)) &&
						!Entity->IsVisible(Utils::GetHitboxPosition(Entity, HITBOX_RIGHT_HAND)) && !Entity->IsVisible(Utils::GetHitboxPosition(Entity, HITBOX_LEFT_FOOT)) &&
						!Entity->IsVisible(Utils::GetHitboxPosition(Entity, HITBOX_RIGHT_FOOT)))
						continue;

				if (Entity->GetTeam() == pLocal->GetTeam())
					glowEntity->set(VisualVars::get().Glow.g_cGlowColorTeammates, VisualVars::get().Glow.g_flGlowScale);
				else 
					glowEntity->set(VisualVars::get().Glow.g_cGlowColor, VisualVars::get().Glow.g_flGlowScale);
			}
			break;

		default:
			if (VisualVars::get().Glow.g_bWeapons && strstr(ent_client_class->m_pNetworkName, XorStr("CWeapon")))
					glowEntity->set(weaponColor, VisualVars::get().Glow.g_flGlowScale);
			break;
		}
	}
}

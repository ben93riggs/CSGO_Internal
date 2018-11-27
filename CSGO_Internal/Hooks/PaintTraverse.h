#pragma once
#include "CRender.h"
#include "Globals.h"
#include "SourceEngine/IMaterial.h"
#include "../AutoWallCrosshair.h"
#include "../RecoilCrosshair.h"
#include "../NadeTracer.h"
#include "../EntityESP.hpp"
#include "../AALines.h"
#include "../DeveloperMessages.h"

#undef DrawText

typedef void(__thiscall* paint_traverse_t)(IPanel*, unsigned int, bool, bool);
paint_traverse_t o_paint_traverse;

inline void __fastcall hooks::paint_traverse(IPanel* ecx, void* edx, int vgui_panel, bool force_repaint, bool allow_force)
{
	o_paint_traverse(ecx, vgui_panel, force_repaint, allow_force);

	if (!MenuVars::get().g_bPaintTraverse)
		return;

	static int overlay_panel;
	static auto lastframecount = 0;
	if (!overlay_panel)
	{
		const auto panelname = VGUIPANEL->GetName(vgui_panel);
		if (panelname[0] == 'M' && panelname[2] == 't')
			overlay_panel = vgui_panel;
	}

	if (!overlay_panel || overlay_panel != vgui_panel || lastframecount == GLOBALS->framecount)
		return;

	lastframecount = GLOBALS->framecount;

	if (ENGINE->IsTakingScreenshot())
		return;

	ENGINE->GetScreenSize(g::screenx, g::screeny);

	if (!g::screeny || !g::screenx)
		return;

	if (!g::centerx || !g::centery)
	{
		g::centerx = g::screenx / 2;
		g::centery = g::screeny / 2;
	}

	if (!MenuVars::get().g_bHideTitle && !MenuVars::get().g_bMainWindow)
		CRender::get().DrawTextMedium(g::centerx, 1, Color::MediumPurple(), true, XorStr("csgo_internal"));

	DevMsg::get().Draw();

	C_CSPlayer * p_local;
	if (!ENGINE->IsInGame() || !ENGINE->IsConnected() || !((p_local = C_CSPlayer::GetLocalPlayer())))
		return;

	Interface->MoveHelper()->SetHost(p_local);
	CRender::get().DrawTextLarge(20, 20, Color::Green(), false, XorStr("MaxSpeed: %0.1f"), Interface->MoveHelper()->m_flMaxSpeed);
	Interface->MoveHelper()->SetHost(nullptr);
	
	AALines::get().PaintTraverse(p_local);
	AALinesEnemy::get().PaintTraverse(p_local);

	if (MiscVars::get().g_bAutoStrafe)
		CRender::get().DrawTextLarge(20, g::screeny * .5f + 28, p_local->GetVelocity().Length2D() >= 299.f ? Color::Green() : Color::Red(), false, XorStr("Velocity: %0.1f"), p_local->GetVelocity().Length2D());

	if (MiscVars::get().Crosshair.g_bHitmarker)
	{
		if (g::hitmarkerlength > 0)
		{
			const uint8_t alpha = g::hitmarkerlength * 5.666666666666667f;
			CRender::DrawLine(g::centerx - 12, g::centery - 12, g::centerx - 5, g::centery - 5, Color::White(alpha));
			CRender::DrawLine(g::centerx + 12, g::centery - 15, g::centerx + 5, g::centery - 5, Color::White(alpha));
			CRender::DrawLine(g::centerx - 12, g::centery + 12, g::centerx - 5, g::centery + 5, Color::White(alpha));
			CRender::DrawLine(g::centerx + 12, g::centery + 12, g::centerx + 5, g::centery + 5, Color::White(alpha));

			g::hitmarkerlength--;
		}
	}

	if (VisualVars::get().ESP.g_bGrenadePrediction)
		CCSGrenadeHint::get().Paint(false);

	if (VisualVars::get().ESP.g_bESPEnabled)
	{
		//Iterate over the EntityList
		for (auto i = 0; i <= ENTITYLIST->GetHighestEntityIndex(); i++)
		{
			auto p_entity = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));
			
			if (!p_entity)
				continue;

			const auto ent_client_class = p_entity->GetClientClass();

			if (!ent_client_class)
				continue;

			//Gets the entity by index
			if (p_entity == p_local && !MiscVars::get().g_bThirdPerson)
				continue;

			if (p_entity->IsDormant() && p_local->GetTeam() == p_entity->GetTeam())
				continue;

			if (p_entity->IsDormant() && !VisualVars::get().ESP.g_bDormantESP)
				continue;

			//check player first since we dont have to do any string comparisons
			if (ent_client_class->m_ClassID == CCSPlayer)
			{
				if (!p_entity->IsAlive()/* || pEntity->IsDormant()*/)
					continue; //Skip Dead and Dormant entities

							  //Checks if its an enemy
				if (!VisualVars::get().ESP.g_bShowTeammates) //only check if its an ally if drawteammates is off
					if (p_local->GetTeam() == p_entity->GetTeam())
						continue;

				entity_esp::get().do_player_esp(p_local, p_entity);
				continue;
			}

			if (VisualVars::get().ESP.g_bShowGroundWeapons)
			{
				if (strstr(ent_client_class->m_pNetworkName, XorStr("CWeapon")) ||
					strstr(ent_client_class->m_pNetworkName, XorStr("CDEagle")) ||
					strstr(ent_client_class->m_pNetworkName, XorStr("CAK47")))
				{
					entity_esp::draw_item_esp(p_entity);
					continue;
				}
			}

			if (VisualVars::get().ESP.g_bShowBomb)
			{
				if (strstr(ent_client_class->m_pNetworkName, XorStr("CC4")))
				{
					entity_esp::draw_bomb(p_entity);
					continue;
				}
			}

			if (VisualVars::get().ESP.g_bShowPlantedBomb)
			{
				if (strstr(ent_client_class->m_pNetworkName, XorStr("CPlantedC4")))
				{
					entity_esp::draw_bomb_damage(p_entity, p_local->GetHealth());
					continue;
				}
			}

			if (VisualVars::get().ESP.g_bShowGrenadePaths)
			{
				if (strstr(ent_client_class->m_pNetworkName, XorStr("Projectile")))
				{
					const auto nade_model = p_entity->GetModel();
					if (!nade_model) return;

					const auto hdr = MODELINFO->GetStudioModel(nade_model);
					if (!hdr) return;

					if (!strstr(hdr->name, XorStr("thrown")) && !strstr(hdr->name, XorStr("dropped")))
						return;

					IMaterial* mats[32];
					MODELINFO->GetModelMaterials(nade_model, hdr->numtextures, mats);

					std::string name = XorStr("Grenade");
					auto nade_color = Color::Black();

					for (int k = 0; k < hdr->numtextures; k++)
					{
						IMaterial* mat = mats[k];
						if (!mat) 
							continue;

						if (strstr(mat->GetName(), XorStr("flashbang")))
						{
							nade_color = Color(255, 255, 0, 255); //yellow
							name = XorStr("flash");
							break;
						}
						if (strstr(mat->GetName(), XorStr("m67_grenade")) || strstr(mat->GetName(), XorStr("hegrenade")))
						{
							nade_color = Color(0, 255, 0, 255); //green
							name = XorStr("grenade");
							break;
						}
						if (strstr(mat->GetName(), XorStr("smoke")))
						{
							nade_color = Color(0, 255, 255, 255); //cyan
							name = XorStr("smoke");
							break;
						}
						if (strstr(mat->GetName(), XorStr("decoy")))
						{
							nade_color = Color(155, 155, 155, 255); //grey
							name = XorStr("decoy");
							break;
						}
						if (strstr(mat->GetName(), XorStr("incendiary")) || strstr(mat->GetName(), XorStr("molotov")))
						{
							nade_color = Color(255, 0, 0, 255); //red
							name = XorStr("fire");
							break;
						}
					}

					NadeTracer::AddTracer(p_entity, nade_color, 60);
					NadeTracer::Draw();

					const auto origin = p_entity->GetAbsOrigin();
					Vector screen_origin;
					Math::WorldToScreen(origin, screen_origin);
					CRender::get().DrawText(screen_origin.x, screen_origin.y, nade_color, true, name.c_str());
				}
			}
		} NadeTracer::Clear();
	}

	if (MiscVars::get().g_bFakeLagOnKey && INPUTSYSTEM->IsButtonDown(MiscVars::get().g_kLagKey))
	{
		const int pos_left = g::screenx * .25;
		const int pos_top = g::screeny - (g::screeny * .1);
		const int width = g::screenx * .5;
		const auto height = 15;

		const auto percent = CLIENTSTATE->net_channel->m_nChokedPackets / 15.f;
		const auto percent_clamped = min(percent, 1.f);

		CRender::DrawFilledRect(pos_left - 1, pos_top - 1, width + 2, height + 2, Color::Black());
		CRender::DrawFilledRect(pos_left, pos_top, width * percent_clamped, 15, percent > 1.f ? Color::Red() : Color::Green());
	}

	AutoWallCrosshair::get().PaintTraverse(g::centerx, g::centery);
	RecoilCrosshair::get().PaintTraverse(p_local, g::screeny, g::screenx, g::centerx, g::centery);
}
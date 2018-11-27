#pragma once
#include "../CustomSky.h"
#include "../BuyBot.h"

inline std::string hitgroup_to_string_temp(const int hitgroup)
{
	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		return XorStr("HITGROUP_HEAD");
	case HITGROUP_CHEST:
		return XorStr("HITGROUP_CHEST");
	case HITGROUP_STOMACH:
		return XorStr("HITGROUP_STOMACH");
	case HITGROUP_LEFTARM:
		return XorStr("HITGROUP_LEFTARM");
	case HITGROUP_RIGHTARM:
		return XorStr("HITGROUP_RIGHTARM");
	case HITGROUP_LEFTLEG:
		return XorStr("HITGROUP_LEFTLEG");
	case HITGROUP_RIGHTLEG:
		return XorStr("HITGROUP_RIGHTLEG");
	case HITGROUP_GENERIC:
		return XorStr("HITGROUP_GENERIC");
	case HITGROUP_GEAR:
		return XorStr("HITGROUP_GEAR");
	default:
		return XorStr("HITGROUP_DEFAULT");
	}
}

typedef bool(__thiscall* fire_event_client_side_t)(IGameEventManager2*, IGameEvent*);
fire_event_client_side_t o_fire_event_client_side;
inline bool __fastcall hooks::fire_event_client_side(IGameEventManager2* ecx, void* edx, IGameEvent* g_event)
{
	if (!MenuVars::get().g_bFireEventClientSide)
		return o_fire_event_client_side(ecx, g_event);

	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
		return o_fire_event_client_side(ecx, g_event);

	if (!g_event)
		return o_fire_event_client_side(ecx, g_event);

	// Filter to only the events we're interested in.
	if (!strcmp(g_event->GetName(), XorStr("player_death")))
	{
		if (ENGINE->GetPlayerForUserID(g_event->GetInt(XorStr("attacker"))) == ENGINE->GetLocalPlayer())
		{			
			if (const char* icon_override = g_config.get_icon_override(g_event->GetString(XorStr("weapon"))))
				g_event->SetString(XorStr("weapon"), icon_override);
		}
	}

	if (!strcmp(g_event->GetName(), XorStr("round_start")))
	{
		WorldChams::get().nightmodeupdated = false;
		ASUSWalls::get().asuswallsupdated = false;
		CustomSky::get().Update();
		buy_bot::run();
	}


		return o_fire_event_client_side(ecx, g_event);
}
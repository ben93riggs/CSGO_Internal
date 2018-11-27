#pragma once
#include "BulletTracer.h"
#include "DeveloperMessages.h"

std::vector<IGameEventListener2*> listeners;

#define EVENT_DEBUG_ID_INIT 42

class item_purchase_listener : public IGameEventListener2
{
public:
	item_purchase_listener()
	{
		GAMEEVENTMANAGER->AddListener(this, XorStr("item_purchase"), false);
		listeners.push_back(this);
	}

	~item_purchase_listener()
	{
		GAMEEVENTMANAGER->RemoveListener(this);
	}

	void FireGameEvent(IGameEvent* p_event) override
	{
		const auto p_local = C_CSPlayer::GetLocalPlayer();

		if (!p_local)
			return;

		const auto ent = static_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(
			ENGINE->GetPlayerForUserID(p_event->GetInt(XorStr("userid")))));

		if (!ent)
			return;

		if (ent->GetTeam() == p_local->GetTeam())
			return;

		std::string eventtext = ent->sGetName() + XorStr(" purchased: ") + p_event->GetString(XorStr("weapon"));

		if (MiscVars::get().g_bEventLog)
			DevMsg::get().OutputMessage(Color::Yellow(), eventtext.c_str());
	}

	int GetEventDebugID() override
	{
		return EVENT_DEBUG_ID_INIT; // 42
	}
};

inline std::string hitgroup_to_string(const int hitgroup)
{
	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		return XorStr("head");
	case HITGROUP_CHEST:
		return XorStr("chest");
	case HITGROUP_STOMACH:
		return XorStr("stomach");
	case HITGROUP_LEFTARM:
		return XorStr("left arm");
	case HITGROUP_RIGHTARM:
		return XorStr("right arm");
	case HITGROUP_LEFTLEG:
		return XorStr("left leg");
	case HITGROUP_RIGHTLEG:
		return XorStr("right leg");
	default:
		return XorStr("");
	}
}

inline auto get_hitmarker_sound() -> std::string
{
	switch (rand() % 3)
	{
	case 0:
		return XorStr("physics/metal/soda_can_impact_hard1.wav");
	case 1:
		return XorStr("physics/metal/soda_can_impact_hard2.wav");
	case 2:
		return XorStr("physics/metal/soda_can_impact_hard3.wav");
	default:
		{
		std::cout << XorStr("RAND TOO BIG") << std::endl;
		return XorStr("buttons/button15.wav");
		}
	}
}

class player_hurt_listener : public IGameEventListener2
{
public:
	player_hurt_listener()
	{
		GAMEEVENTMANAGER->AddListener(this, XorStr("player_hurt"), false);
		listeners.push_back(this);
	}

	~player_hurt_listener()
	{
		GAMEEVENTMANAGER->RemoveListener(this);
	}

	void FireGameEvent(IGameEvent* p_event) override
	{
		if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
			return;

		if (!p_event)
			return;

		const auto p_local = C_CSPlayer::GetLocalPlayer();

		if (!p_local)
			return;

		if (!p_local->IsAlive())
			return;

		auto* ent = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(
			ENGINE->GetPlayerForUserID(p_event->GetInt(XorStr("userid")))));

		if (!ent)
			return;

		auto* attacker_ent = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(
			ENGINE->GetPlayerForUserID(p_event->GetInt(XorStr("attacker")))));

		if (!attacker_ent)
			return;

		if (attacker_ent != p_local && ent != p_local)
			return;

		//auto anim_state = G::plist[ent->EntIndex()].Animations.GetAnimState(ent);
		//if (anim_state)
		//	anim_state->DebugShowAnimState();

		const auto hitgroup = p_event->GetInt(XorStr("hitgroup"));

		if (MiscVars::get().g_bEventLog && attacker_ent != ent && attacker_ent == p_local)
		{
			auto hitgroup_str = hitgroup_to_string(hitgroup);
			auto health = ent->GetHealth() - p_event->GetInt(XorStr("dmg_health"));
			if (!ent->IsAlive())
				health = 0;
			if (health < 0)
				health = 0;
			std::string eventtext = XorStr("hit ") + ent->sGetName() + (!hitgroup_str.empty() ? XorStr( "'s " ) + hitgroup_str : "") + XorStr(" for ") + p_event->GetString(XorStr("dmg_health")) + XorStr(" damage. (") + std::to_string(health) + XorStr( " remaining)" );
			DevMsg::get().OutputMessage(Color::White(), eventtext.c_str());
		}

		if (VisualVars::get().ESP.g_bDamage)
		{
			if (ent != p_local)
				DEBUGOVERLAY->AddEntityTextOverlay(ent->EntIndex(), 0, 1.5f, 255, 0, 0, 255, XorStr("%d"), p_event->GetInt(XorStr("dmg_health")));
		}

		if (attacker_ent != p_local)
			return;

		if (MiscVars::get().Crosshair.g_bHitmarker)
		{
			g::hitmarkerlength = 45;
			if (MiscVars::get().Crosshair.g_bHitmarkerSound)
				MATSURFACE->PlaySound2(get_hitmarker_sound().c_str()); //buttons/arena_switch_press_02.wav
		}

		Accuracy::get().PlayerHurt(ent);
	}

	int GetEventDebugID() override
	{
		return EVENT_DEBUG_ID_INIT; // 42
	}
};

class bullet_impact_listener : public IGameEventListener2
{
public:
	bullet_impact_listener()
	{
		GAMEEVENTMANAGER->AddListener(this, XorStr("bullet_impact"), false);
		listeners.push_back(this);
	}

	~bullet_impact_listener()
	{
		GAMEEVENTMANAGER->RemoveListener(this);
	}

	void FireGameEvent(IGameEvent* p_event) override
	{

		if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
			return;

		const auto p_local = C_CSPlayer::GetLocalPlayer();

		if (!p_local)
			return;

		auto* ent = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(
			ENGINE->GetPlayerForUserID(p_event->GetInt(XorStr("userid")))));

		if (!ent)
			return;

		const auto bullet_hit_pos = Vector(p_event->GetFloat(XorStr("x")), p_event->GetFloat(XorStr("y")), p_event->GetFloat(XorStr("z")));

		if (ent == p_local)
			Accuracy::get().ShotReceived(bullet_hit_pos);

		const auto enemy = ent->GetTeam() != p_local->GetTeam();

		if (!enemy && ent != p_local)
			return;

		if (!VisualVars::get().ESP.g_bBulletTracer)
			return;

		BulletBeams::DrawBeam(ent->GetEyePos(), bullet_hit_pos, ent == p_local ? Color::LightSalmon() : Color::Orange());
		DEBUGOVERLAY->AddBoxOverlay(bullet_hit_pos, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0,0,0), 255, 0, 0, 150, 1.5f);
	}

	int GetEventDebugID() override
	{
		return EVENT_DEBUG_ID_INIT; // 42
	}
};

extern player_hurt_listener* g_pPlayerHurtListener;
extern bullet_impact_listener* g_pBulletImpactListener;
extern item_purchase_listener* g_pItemPurchaseListener;

player_hurt_listener* g_pPlayerHurtListener = nullptr;
bullet_impact_listener* g_pBulletImpactListener = nullptr;
item_purchase_listener* g_pItemPurchaseListener = nullptr;
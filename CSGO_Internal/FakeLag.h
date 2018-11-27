#pragma once
#include "Singleton.h"
#include "Globals.h"
#include "Autowall.h"

class peeking_fakelag_autowall : public singleton<peeking_fakelag_autowall>
{
public:
	static void scale_damage(C_CSPlayer* target, const CCSWeaponInfo* weapon_data, float& current_damage);
	static bool trace_to_exit(CGameTrace& enter_trace, CGameTrace& exit_trace, Vector start_position, Vector direction);
	bool handle_bullet_penetration(C_CSPlayer* local, const CCSWeaponInfo* weapon_data, CGameTrace& enter_trace, Vector& eye_position, Vector direction, int& possible_hits_remaining, float& current_damage, float penetration_power, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration) const;
	bool fire_bullet(C_CSPlayer* local, CBaseCombatWeapon* weapon, float& max_range, Vector& direction, float& current_damage, int& possible_hits_remaining) const;
	float can_hit(Vector& end_pos, C_CSPlayer* local) const;
};

class fake_lag : public singleton<fake_lag>
{
	static bool adaptive_fl(C_CSPlayer* local, bool& send_packet);
	static bool static_fl(bool& send_packet);
	static bool peeking_fl(C_CSPlayer* local, bool& send_packet);

public:
	static bool create_move(bool& send_packet);
};

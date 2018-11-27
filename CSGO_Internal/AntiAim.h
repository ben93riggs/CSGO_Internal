#pragma once
#include "Globals.h"
#include "Singleton.h"

class anti_aim : public singleton<anti_aim>
{
	static void at_targets(C_CSPlayer* local);
	static bool should_aa(C_CSPlayer* local);

	bool break_lower_body_ = false;
	float next_body_update_ = 0.f;

public:

	void update_lby_animations();
	void break_lby(float angle, bool& send_packet, C_CSPlayer* local) const;
	static void accelerate(C_CSPlayer* player, Vector& wishdir, float wishspeed, float accel, Vector& out_vel);
	void walk_move(C_CSPlayer* player, Vector& out_vel) const;
	static void friction(C_CSPlayer* local, Vector& out_vel);
	void fakewalk(CUserCmd* cmd, bool& send_packet) const;
	void create_move(bool& ret, bool& send_packet) const;
};

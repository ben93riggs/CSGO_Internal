#include "AntiAim.h"
#include "TickFix.h"
#include "Freestanding.h"

void anti_aim::update_lby_animations()
{
	if (!ENGINE->IsInGame())
		return;

	if (!ENGINE->IsConnected())
		return;

	const auto local = C_CSPlayer::GetLocalPlayer();

	if (!local)
		return;

	if (CLIENTSTATE->m_nDeltaTick <= 0)
		return;

	auto velocity = (local->GetOrigin() - local->GetOldOrigin()) / (local->GetSimulationTime() - local->m_flOldSimulationTime());

	const auto cur_time = TICKS_TO_TIME(TickFix::get().GetTickbase(g::cmd, local));
	if (!CLIENTSTATE->chokedcommands)
	{
		if (velocity.Length() > 0.1f && (local->GetFlags() & FL_ONGROUND) && !g::b_in_fake_walk)
			next_body_update_ = cur_time + 0.22f;
		else if (cur_time >= next_body_update_)
		{
			next_body_update_ = cur_time + 1.1f;
		}
	}

	break_lower_body_ = (local->GetFlags() & FL_ONGROUND) && ((next_body_update_ - cur_time) <= GLOBALS->interval_per_tick);
}

void anti_aim::break_lby(const float angle, bool& send_packet, C_CSPlayer* local) const
{
	if (!local)
		return;

	if (!AAVars::get().aa[g::i_current_aa].g_bAntiLBY)
		return;

	if (!break_lower_body_)
		return;

	if (send_packet)
		return;

	g::cmd->viewangles.y = g::anti_aim_angles.y - angle;
}

void anti_aim::accelerate(C_CSPlayer* player, Vector& wishdir, const float wishspeed, const float accel, Vector& out_vel)
{
	// See if we are changing direction a bit
	const auto currentspeed = out_vel.Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	const auto addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
		return;

	// Determine amount of accleration.
	float accelspeed = accel * GLOBALS->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	for (auto i = 0; i < 3; i++)
		out_vel[i] += accelspeed * wishdir[i];
}

void anti_aim::walk_move(C_CSPlayer* player, Vector& out_vel) const
{
	Vector forward, right, up, wishvel;

	Math::AngleVectors(player->GetEyeAngles(), forward, right, up);  // Determine movement angles

																	 // Copy movement amounts
	Interface->MoveHelper()->SetHost(player);
	const float_t fmove = Interface->MoveHelper()->m_flForwardMove;
	const float_t smove = Interface->MoveHelper()->m_flSideMove;
	Interface->MoveHelper()->SetHost(nullptr);

	if (forward[2] != 0)
	{
		forward[2] = 0;
		Math::NormalizeVector(forward);
	}

	if (right[2] != 0)
	{
		right[2] = 0;
		Math::NormalizeVector(right);
	}

	for (auto i = 0; i < 2; i++)	// Determine x and y parts of velocity
		wishvel[i] = forward[i] * fmove + right[i] * smove;

	wishvel[2] = 0;	// Zero out z part of velocity

	auto wishdir = wishvel; // Determine maginitude of speed of move
	float_t wishspeed = wishdir.Normalize2();

	// Clamp to server defined max speed
	Interface->MoveHelper()->SetHost(player);
	if ((wishspeed != 0.0f) && (wishspeed > Interface->MoveHelper()->m_flMaxSpeed))
	{
		VectorMultiply(wishvel, player->m_flMaxspeed() / wishspeed, wishvel);
		wishspeed = player->m_flMaxspeed();
	}
	Interface->MoveHelper()->SetHost(nullptr);
	// Set pmove velocity
	out_vel[2] = 0;
	accelerate(player, wishdir, wishspeed, CVAR->FindVar(XorStr("sv_accelerate"))->GetFloat(), out_vel);
	out_vel[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(out_vel, player->m_vecBaseVelocity(), out_vel);

	const auto spd = out_vel.Length();

	if (spd < 1.0f)
	{
		out_vel.Init();
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(out_vel, player->m_vecBaseVelocity(), out_vel);
		return;
	}

	Interface->MoveHelper()->SetHost(player);
	Interface->MoveHelper()->m_outWishVel += wishdir * wishspeed;
	Interface->MoveHelper()->SetHost(nullptr);

	// Don't walk up stairs if not on ground.
	if (!(player->GetFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(out_vel, player->m_vecBaseVelocity(), out_vel);
		return;
	}

	// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(out_vel, player->m_vecBaseVelocity(), out_vel);
}

void anti_aim::friction(C_CSPlayer* local, Vector& out_vel)
{
	const auto speed = out_vel.Length();

	if (speed < 0.1f)
		return;

	float drop = 0;

	// apply ground friction
	if (local->GetFlags() & FL_ONGROUND)
	{
		const float friction = CVAR->FindVar(XorStr("sv_friction"))->GetFloat() * local->m_surfaceFriction();

		// Bleed off some speed, but if we have less than the bleed
		// threshold, bleed the threshold amount.

		const auto control = (speed < CVAR->FindVar(XorStr("sv_stopspeed"))->GetFloat()) ? CVAR->FindVar(XorStr("sv_stopspeed"))->GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * GLOBALS->frametime;
	}

	auto newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;
		// Adjust velocity according to proportion.
		VectorMultiply(out_vel, newspeed, out_vel);
	}
}

void anti_aim::fakewalk(CUserCmd* cmd, bool& send_packet) const
{
	g::b_in_fake_walk = false;

	if (!MenuVars::get().g_bFakewalk || !INPUTSYSTEM->IsButtonDown(MenuVars::get().g_kFakeWalkKey))
		return;

	const auto local = C_CSPlayer::GetLocalPlayer();

	if (!local)
		return;

	if (!local->IsAlive())
		return;

	if (!(local->GetFlags() & FL_ONGROUND))
		return;

	send_packet = true;

	auto velocity = g::velocity_unpredicted;

	const auto ticks_to_update = TIME_TO_TICKS(next_body_update_ - TICKS_TO_TIME(TickFix::get().GetTickbase(cmd, local))) - 1;
	int32_t ticks_to_stop;
	for (ticks_to_stop = 0; ticks_to_stop < 15; ticks_to_stop++)
	{
		if (velocity.Length2D() < 0.1f)
			break;

		if (local->GetFlags() & FL_ONGROUND)
		{
			velocity[2] = 0.0f;
			friction(local, velocity);
			walk_move(local, velocity);
		}
	}

	const auto max_ticks = std::min<int32_t>(7, ticks_to_update);
	const int32_t choked = CLIENTSTATE->chokedcommands;
	const auto ticks_left = max_ticks - choked;

	send_packet = !(choked < max_ticks || ticks_to_stop);

	if (ticks_to_stop > ticks_left || !choked || send_packet)
	{
		const float_t speed = velocity.Length();

		if (speed > 13.f)
		{
			QAngle direction;
			Math::VectorAngles(velocity, direction);

			direction.y = cmd->viewangles.y - direction.y;

			Vector forward;
			Math::AngleVectors(direction, &forward);
			const auto negated_direction = forward * -speed;

			cmd->forwardmove = negated_direction.x;
			cmd->sidemove = negated_direction.y;
		}
		else
		{
			cmd->forwardmove = 0.f;
			cmd->sidemove = 0.f;
		}
	}

	g::b_in_fake_walk = true;
}

void anti_aim::at_targets(C_CSPlayer* local)
{
	if (!AAVars::get().aa[g::i_current_aa].g_bAtTargets)
		return;

	auto lowest = 99999999.f;
	const auto eye_pos = local->GetEyePos();

	for (auto i = 0; i <= 64; i++)
	{
		if (i == local->EntIndex()) continue;
		auto entity = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));
		if (!entity) continue;
		if (entity->IsDormant()) continue;
		if (!entity->IsAlive()) continue;
		if (entity->GetTeam() == local->GetTeam()) continue;
		const auto client_class = entity->GetClientClass();
		if (!client_class) continue;
		if (client_class->m_ClassID != CCSPlayer) continue;

		if (entity->GetEyePos().DistToSqr(eye_pos) < lowest)
		{
			lowest = entity->GetEyePos().DistTo(eye_pos);
			g::cmd->viewangles.y = Math::CalcAngle(eye_pos, entity->GetEyePos()).y;
		}
	}
}

bool anti_aim::should_aa(C_CSPlayer* local)
{
	if (!local)
		return false;

	if (!local->IsAlive())
		return false;

	if (globals::get().knifebotting)
		return false;

	auto weapon = local->GetActiveWeapon();

	if (!weapon)
		return false;

	if (weapon->IsNadeThrown(g::cmd->hasbeenpredicted ? GLOBALS->curtime : GLOBALS->curtime + GLOBALS->interval_per_tick))
		return false;

	if (local->GetMoveType() == MOVETYPE_LADDER || local->GetMoveType() == MOVETYPE_NOCLIP)
		return false;

	if (g::cmd->buttons & IN_USE)
		return false;

	if (!weapon->IsGrenade())
	{
		if (local->bCanFire())
		{
			if (INPUTSYSTEM->IsButtonDown(MOUSE_LEFT) || g::cmd->buttons & IN_ATTACK)
				return false;

			if ((INPUTSYSTEM->IsButtonDown(static_cast<ButtonCode_t>(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].menu.g_kAimKey)) || INPUTSYSTEM->IsButtonDown(static_cast<ButtonCode_t>(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].menu.g_kTriggerKey))))
				return false;

			if (g::aimbotting)
				return false;
		}
	}

	return true;
}

class pitch_aa
{
	enum pitch_aa_list
	{
		pitch_none,
		pitch_zero,
		pitch_emotion_up,
		pitch_emotion_down,
		pitch_emotion_jitter,
	};

	static void none() {}
	static void zero()
	{
		g::cmd->viewangles.x = 0;
	}
	static void emotion_up()
	{
		g::cmd->viewangles.x = -89;
	}
	static void emotion_down()
	{
		g::cmd->viewangles.x = 89;
	}
	static void emotion_jitter(const bool flip)
	{
		g::cmd->viewangles.x = flip ? 89 : -89;
	}
public:
	static void do_pitch_aa(const bool flip)
	{
		switch (AAVars::get().aa[g::i_current_aa].g_iAAPitch)
		{
		case pitch_none:
			none();
			break;
		case pitch_emotion_up:
			emotion_up();
			break;
		case pitch_emotion_down:
			emotion_down();
			break;
		case pitch_emotion_jitter:
			emotion_jitter(flip);
			break;
		default: 
			break;
		}
	}
};

class yaw_aa
{
	enum yaw_aa_list
	{
		yaw_none,
		yaw_forwards,
		yaw_backwards,
		yaw_jitter,
		yaw_sideways,
		yaw_spin,
		yaw_lowerbody,
		yaw_velocityangle,
		yaw_lbyspin
	};

	static void none() {}
	static void forwards(const int offset)
	{
		g::cmd->viewangles.y += offset;//not necessary to set an angle here
	}
	static void backwards(const int offset)
	{
		g::cmd->viewangles.y += 180 + offset;
	}
	static void jitter(const bool fake, const bool flip, const float offset, const bool is_moving = false)
	{
		g::cmd->viewangles.y += 90;

		if (fake)
		{
			if (!is_moving)
				g::cmd->viewangles.y = flip ? g::cmd->viewangles.y + offset + (AAVars::get().aa[g::i_current_aa].g_iJitterRangeFirst * 0.5f) : g::cmd->viewangles.y + offset - (AAVars::get().aa[g::i_current_aa].g_iJitterRangeFirst * 0.5f);
			else
				g::cmd->viewangles.y = flip ? g::cmd->viewangles.y + offset + (AAVars::get().aa[g::i_current_aa].g_iMovingJitterRange * 0.5f) : g::cmd->viewangles.y + offset - (AAVars::get().aa[g::i_current_aa].g_iMovingJitterRange * 0.5f);
		}
		else
			if (!is_moving)
				g::cmd->viewangles.y = flip ? g::cmd->viewangles.y + offset + (AAVars::get().aa[g::i_current_aa].g_iJitterRangeSecond * 0.5f) : g::cmd->viewangles.y + offset - (AAVars::get().aa[g::i_current_aa].g_iJitterRangeSecond * 0.5f);
	}
	static void sideways(const int offset)
	{
		g::cmd->viewangles.y += 90 + offset;
	}
	static void spin(const int offset)
	{
		g::cmd->viewangles.y += fmodf(GLOBALS->curtime, 3.6f) * 2100.f + offset;
	}
	static void lower_body(C_CSPlayer* local, const int offset)
	{
		g::cmd->viewangles.y = local->GetLowerBodyYawTarget() + offset;
	}
	static void velocity_angle(C_CSPlayer* local, const int offset)
	{
		QAngle direction;
		Math::VectorAngles(local->GetVelocity(), direction);
		Math::GUWOPNormalizeYaw360(direction.y);
		g::cmd->viewangles.y = direction.y + 180.f + offset;
	}
	static void lby_spin(C_CSPlayer* local, const int offset)
	{
		const auto ticks_til_update = ((1.1f + GLOBALS->interval_per_tick * 1.6f) / GLOBALS->interval_per_tick);

		g::cmd->viewangles.y = 360.f / ticks_til_update * GLOBALS->tickcount + offset;
	}

public:
	static void do_yaw_aa(C_CSPlayer* local, const bool real_angle_this_tick, const bool realflip, const bool fakeflip, bool& send_packet)
	{
		send_packet = AAVars::get().aa[g::i_current_aa].g_bShouldFake ? !real_angle_this_tick : true;

		if (real_angle_this_tick) //not sending packet
		{
			const auto is_moving = local->GetVelocity().Length2D() > 1.0f && AAVars::get().aa[g::i_current_aa].g_iMovingAA != yaw_none;
			const auto offset = is_moving ? AAVars::get().aa[g::i_current_aa].g_iMovingAAOffset : AAVars::get().aa[g::i_current_aa].g_iAARealYawOffset;
			
			//do real angle
			switch (is_moving ? AAVars::get().aa[g::i_current_aa].g_iMovingAA : AAVars::get().aa[g::i_current_aa].g_iAAYaw)
			{
			case yaw_none:
				none();
				break;
			case yaw_forwards:
				forwards(offset);
				break;
			case yaw_backwards:
				backwards(offset);
				break;
			case yaw_jitter:
				jitter(true, realflip, offset, is_moving);
				break;
			case yaw_sideways:
				sideways(offset);
				break;
			case yaw_spin:
				spin(offset);
				break;
			case yaw_lowerbody:
				lower_body(local, offset);
				break;
			case yaw_velocityangle:
				velocity_angle(local, offset);
				break;
			case yaw_lbyspin:
				lby_spin(local, offset);
				break;
			default:
				break;
			}

			Math::Clamp(g::cmd->viewangles);
		}
		else //sending packet
		{
			const auto offset = AAVars::get().aa[g::i_current_aa].g_iAAFakeYawOffset;

			//do fake angle
			switch (AAVars::get().aa[g::i_current_aa].g_iAAFakeYaw)
			{
			case yaw_none:
				none();
				break;
			case yaw_forwards:
				forwards(offset);
				break;
			case yaw_backwards:
				backwards(offset);
				break;
			case yaw_jitter:
				jitter(false, fakeflip, offset);
				break;
			case yaw_sideways:
				sideways(offset);
				break;
			case yaw_spin:
				spin(offset);
				break;
			case yaw_lowerbody:
				lower_body(local, offset);
				break;
			case yaw_velocityangle:
				velocity_angle(local, offset);
				break;
			case yaw_lbyspin:
				lby_spin(local, offset);
				break;
			default:
				break;
			}

			Math::Clamp(g::cmd->viewangles);
		}
	}
};

void update_anims(C_CSPlayer* local, const bool send_packet) {
	static std::array<C_AnimationLayer, 15> layer_records{};
	static auto init = false;
	const auto layer_count = local->GetNumAnimOverlays();
	if(init) {
		for (auto i = 0; i < layer_count; i++)
		{
			auto& current_layer = local->GetAnimOverlay2(i);
			current_layer.m_nOrder = layer_records.at(i).m_nOrder;
			current_layer.m_nSequence = layer_records.at(i).m_nSequence;
			current_layer.m_flWeight = layer_records.at(i).m_flWeight;
			current_layer.m_flCycle = layer_records.at(i).m_flCycle;
		}
	}

	init = true;

	if (send_packet) {
		auto anim_state = local->GetPlayerAnimState();

		if (!anim_state)
			return;

		local->m_bClientSideAnimation() = false;
		anim_state->m_flLastClientSideAnimationUpdateTime = 0;
		anim_state->m_iLastClientSideAnimationUpdateFramecount = 0;
		anim_state->UpdateAnimationState(g::cmd->viewangles);
		local->UpdateClientSideAnimation();
	}

	for (auto i = 0; i < layer_count; i++)
	{
		auto& current_layer = local->GetAnimOverlay2(i);
		layer_records.at(i).m_nOrder = current_layer.m_nOrder;
		layer_records.at(i).m_nSequence = current_layer.m_nSequence;
		layer_records.at(i).m_flWeight = current_layer.m_flWeight;
		layer_records.at(i).m_flCycle = current_layer.m_flCycle;
	}
}

void anti_aim::create_move(bool& ret, bool& send_packet) const
{
	const auto local = C_CSPlayer::GetLocalPlayer();

	if (!local)
		return;

	if (!AAVars::get().g_bAAEnabled || MenuVars::get().g_iCurrentAimbotMode == 0)
		return;

	static bool flip, realflip, fakeflip;

	if (g::fakelagging)
		flip = !send_packet;
	else
		flip = !flip;


	if (flip)
		realflip = !realflip; //if we are sending a packet we flip our fakeflip
	else
		fakeflip = !fakeflip; //otherwise we flip our realflip

	if (!should_aa(local))
		return;

	at_targets(local);
	pitch_aa::do_pitch_aa(flip);
	yaw_aa::do_yaw_aa(local, flip, realflip, fakeflip, send_packet);

	if (flip) //if this is a real angle
		freestanding::get().edge_anti_aim(g::cmd);

	break_lby(AAVars::get().aa[g::i_current_aa].g_flAntiLBYOffset, send_packet, local);

	ret = false;

	Math::Clamp(g::cmd->viewangles);
}


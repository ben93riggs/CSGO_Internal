#include "FakeLag.h"
#include "Autowall.h"

void peeking_fakelag_autowall::scale_damage(C_CSPlayer* target, const CCSWeaponInfo* weapon_data, float& current_damage)
{
	const auto armor_value = target->GetArmorValue();
	current_damage *= 4.f;

	if (armor_value > 0 && target->HasHelmet())
	{
		const auto bonus_value = 1.f;
		const auto armor_bonus_ratio = 0.5f;
		const auto armor_ratio = weapon_data->flArmorRatio / 2.f;

		auto new_damage = current_damage * armor_ratio;

		if (((current_damage - (current_damage * armor_ratio)) * (bonus_value * armor_bonus_ratio)) > armor_value)
			new_damage = current_damage - (armor_value / armor_bonus_ratio);

		current_damage = new_damage;
	}
}

bool peeking_fakelag_autowall::trace_to_exit(CGameTrace& enter_trace, CGameTrace& exit_trace, Vector start_position, Vector direction)
{
	const auto max_distance = 90.f;
	const auto ray_extension = 4.f;
	float current_distance = 0;
	auto first_contents = 0;

	while (current_distance <= max_distance)
	{
		current_distance += ray_extension; ////Add extra distance to our ray

		//Multiply the direction vector to the distance so we go outwards, add our position to it.
		auto start = start_position + direction * current_distance;

		if (!first_contents)
			first_contents = ENGINETRACE->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		const auto point_contents = ENGINETRACE->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (!(point_contents & MASK_SHOT_HULL) || point_contents & CONTENTS_HITBOX && point_contents != first_contents)
		{
			//Let's setup our end position by deducting the direction by the extra added distance
			auto end = start - (direction * ray_extension);

			//Let's cast a ray from our start pos to the end pos
			CAutoWall::TraceLine(start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exit_trace);

			//Let's check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if (exit_trace.startsolid && exit_trace.surface.flags & SURF_HITBOX)
			{
				CAutoWall::TraceLine(start, start_position, MASK_SHOT_HULL, exit_trace.m_pEnt, &exit_trace);

				if (exit_trace.DidHit() && !exit_trace.startsolid)
					return true;

				continue;
			}

			//Can we hit? Is the wall solid?
			if (exit_trace.DidHit() && !exit_trace.startsolid)
			{
				//Is the wall a breakable? If so, let's shoot through it.
				if (dynamic_cast<C_CSPlayer*>(enter_trace.m_pEnt)->IsBreakableEnt() && (dynamic_cast<C_CSPlayer*>(exit_trace.m_pEnt)->IsBreakableEnt()))
					return true;

				if (enter_trace.surface.flags & SURF_NODRAW || !(exit_trace.surface.flags & SURF_NODRAW) && (exit_trace.plane.normal.Dot(direction) <= 1.f))
					return true;

				continue;
			}

			if (!exit_trace.DidHit() || exit_trace.startsolid)
			{
				if (CAutoWall::DidHitNonWorldEntity(enter_trace.m_pEnt) && dynamic_cast<C_CSPlayer*>(enter_trace.m_pEnt)->IsBreakableEnt())
				{
					exit_trace = enter_trace;
					exit_trace.endpos = start + direction;
					return true;
				}
			}
		}
	}
	return false;
}

bool peeking_fakelag_autowall::handle_bullet_penetration(C_CSPlayer* local, const CCSWeaponInfo* weapon_data, CGameTrace& enter_trace, Vector& eye_position, const Vector direction, int& possible_hits_remaining, float& current_damage, const float penetration_power, const bool sv_penetration_type, const float ff_damage_reduction_bullets, const float ff_damage_bullet_penetration) const
{
	trace_t exit_trace;

	const auto target = C_CSPlayer::GetLocalPlayer();

	if (!target)
		return false;

	const auto enter_surface_data = PHYSPROPS->GetSurfaceData(enter_trace.surface.surfaceProps);
	const int enter_material = enter_surface_data->game.material;
	const auto enter_surf_penetration_modifier = enter_surface_data->game.flPenetrationModifier;

	const auto enter_damage_modifier = enter_surface_data->game.flDamageModifier;
	float thickness, final_damage_modifier, combined_penetration_modifier;
	const bool is_solid_surf = ((enter_trace.contents >> 3) & CONTENTS_SOLID);
	const bool is_light_surf = ((enter_trace.surface.flags >> 7) & SURF_LIGHT);

	if (possible_hits_remaining <= 0
		|| (!possible_hits_remaining && !is_light_surf && !is_solid_surf && enter_material != CHAR_TEX_GRATE && enter_material != CHAR_TEX_GLASS)
		|| weapon_data->flPenetration <= 0.f
		|| !trace_to_exit(enter_trace, exit_trace, enter_trace.endpos, direction)
		&& !(ENGINETRACE->GetPointContents(enter_trace.endpos, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
		return false;

	const auto exit_surface_data = PHYSPROPS->GetSurfaceData(exit_trace.surface.surfaceProps);

	if (!exit_surface_data)
		return false;

	const int exit_material = exit_surface_data->game.material;
	const auto exit_surf_penetration_modifier = exit_surface_data->game.flPenetrationModifier;
	const auto exit_damage_modifier = exit_surface_data->game.flDamageModifier;

	//Are we using the newer penetration system?
	if (sv_penetration_type)
	{
		if (enter_material == CHAR_TEX_GRATE || enter_material == CHAR_TEX_GLASS)
		{
			combined_penetration_modifier = 3.f;
			final_damage_modifier = 0.05f;
		}
		else if (is_solid_surf || is_light_surf)
		{
			combined_penetration_modifier = 1.f;
			final_damage_modifier = 0.16f;
		}
		else if (enter_material == CHAR_TEX_FLESH && (local->GetTeam() == target->GetTeam() && ff_damage_reduction_bullets == 0.f))
		{
			//Look's like you aren't shooting through your teammate today
			if (ff_damage_bullet_penetration == 0.f)
				return false;

			//Let's shoot through teammates and get kicked for teamdmg! Whatever, atleast we did damage to the enemy. I call that a win.
			combined_penetration_modifier = ff_damage_bullet_penetration;
			final_damage_modifier = 0.16f;
		}
		else
		{
			combined_penetration_modifier = (enter_surf_penetration_modifier + exit_surf_penetration_modifier) / 2.f;
			final_damage_modifier = 0.16f;
		}

		//Do our materials line up?
		if (enter_material == exit_material)
		{
			if (exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD)
				combined_penetration_modifier = 3.f;
			else if (exit_material == CHAR_TEX_PLASTIC)
				combined_penetration_modifier = 2.f;
		}

		//Calculate thickness of the wall by getting the length of the range of the trace and squaring
		thickness = (exit_trace.endpos - enter_trace.endpos).LengthSqr();
		const auto modifier = fmaxf(1.f / combined_penetration_modifier, 0.f);

		//This calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
		const auto lost_damage = fmaxf(
			((modifier * thickness) / 24.f)
			+ ((current_damage * final_damage_modifier)
				+ (fmaxf(3.75f / penetration_power, 0.f) * 3.f * modifier)), 0.f);

		//Did we loose too much damage?
		if (lost_damage > current_damage)
			return false;

		//We can't use any of the damage that we've lost
		if (lost_damage > 0.f)
			current_damage -= lost_damage;

		//Do we still have enough damage to deal?
		if (current_damage < 1.f)
			return false;

		eye_position = exit_trace.endpos;
		--possible_hits_remaining;

		return true;
	}
	//else

	//Legacy penetration system
	combined_penetration_modifier = 1.f;

	if (is_solid_surf || is_light_surf)
		final_damage_modifier = 0.99f; //Good meme :^)
	else
	{
		final_damage_modifier = fminf(enter_damage_modifier, exit_damage_modifier);
		combined_penetration_modifier = fminf(enter_surf_penetration_modifier, exit_surf_penetration_modifier);
	}

	if (enter_material == exit_material && (exit_material == CHAR_TEX_METAL || exit_material == CHAR_TEX_WOOD))
		combined_penetration_modifier += combined_penetration_modifier;

	thickness = (exit_trace.endpos - enter_trace.endpos).LengthSqr();

	if (sqrt(thickness) <= combined_penetration_modifier * penetration_power)
	{
		current_damage *= final_damage_modifier;
		eye_position = exit_trace.endpos;
		--possible_hits_remaining;

		return true;
	}

	return false;
}

bool peeking_fakelag_autowall::fire_bullet(C_CSPlayer* local, CBaseCombatWeapon* weapon, float& max_range, Vector& direction, float& current_damage, int& possible_hits_remaining) const
{
	const auto target = C_CSPlayer::GetLocalPlayer();

	if (!target)
		return false;

	if (!weapon)
		return false;

	static auto util_clip_trace_to_players = [](const Vector& vec_abs_start, const Vector& vec_abs_end, unsigned int mask, ITraceFilter* filter, trace_t* tr)
	{
		static uintptr_t clptrtp = Utils::FindSignature(Utils::get_client_dll(), XorStr("53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B 04 89 6C 24 ? 8B EC 81 EC ? ? ? ? 8B 43 10 56 57 52 F3 0F 10 40"));

		if (!clptrtp)
			return false;

		__asm {
			push tr
			push filter
			push mask
			lea edx, vec_abs_end
			lea ecx, vec_abs_start
			call clptrtp
			add esp, 0xC
		}
	};

	//	  Current bullet travel Power to penetrate Distance to penetrate Range               Player bullet reduction convars			  Amount to extend ray by
	float current_distance = 0.f, penetration_power, penetration_distance;
	const auto ray_extension = 40.f;

	auto eye_position = local->GetEyePos();

	//For being superiour when the server owners think your autowall isn't well reversed. Imagine a meme HvH server with the old penetration system- pff
	static auto penetration_system = CVAR->FindVar(XorStr("sv_penetration_type"));
	static auto damage_reduction_bullets = CVAR->FindVar(XorStr("ff_damage_reduction_bullets"));
	static auto damage_bullet_penetration = CVAR->FindVar(XorStr("ff_damage_bullet_penetration"));

	if (!penetration_system || !damage_reduction_bullets || !damage_bullet_penetration)
		return false;

	const auto sv_penetration_type = penetration_system->GetBool();
	const auto ff_damage_reduction_bullets = damage_reduction_bullets->GetFloat();
	const auto ff_damage_bullet_penetration = damage_bullet_penetration->GetFloat();

	const auto weapon_data = weapon->GetCSWpnData();

	if (!weapon_data)
		return false;

	CAutoWall::GetBulletTypeParameters(penetration_power, penetration_distance, weapon_data->szBulletType, sv_penetration_type);

	if (sv_penetration_type)
		penetration_power = weapon_data->flPenetration;

	//This gets set in FX_Firebullets to 4 as a pass-through value.
	//CS:GO has a maximum of 4 surfaces a bullet can pass-through before it 100% stops.
	//Excerpt from Valve: https://steamcommunity.com/sharedfiles/filedetails/?id=275573090
	//"The total number of surfaces any bullet can penetrate in a single flight is capped at 4." -CS:GO Official

	//Set our current damage to what our gun's initial damage reports it will do
	current_damage = weapon_data->iDamage;

	trace_t enter_trace;
	CTraceFilter filter;
	filter.pSkip = local;

	//If our damage is greater than (or equal to) 1, and we can shoot, let's shoot.
	while (possible_hits_remaining > 0 && current_damage >= 1.f)
	{
		max_range -= current_distance; //Calculate max bullet range

		const auto end = eye_position + direction * max_range; //Create endpoint of bullet

		Utils::TraceLine(eye_position, end, MASK_SHOT_HULL | CONTENTS_HITBOX, local, &enter_trace);
		util_clip_trace_to_players(eye_position, end + direction * ray_extension, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enter_trace);

		const auto enter_surface_data = PHYSPROPS->GetSurfaceData(enter_trace.surface.surfaceProps); //We have to do this *after* tracing to the player.

		if (!enter_surface_data)
			return false;

		const auto enter_surf_penetration_modifier = enter_surface_data->game.flPenetrationModifier;

		if (enter_trace.fraction == 1.f || enter_trace.m_pEnt == target) //"Fraction == 1" means that we didn't hit anything. We don't want that- so let's break on it.
		{
			scale_damage(target, weapon_data, current_damage);
			return true;
		}

		//calculate the damage based on the distance the bullet traveled.
		current_distance += enter_trace.fraction * max_range;

		//Let's make our damage drops off the further away the bullet is.
		current_damage *= pow(weapon_data->flRangeModifier, current_distance / 500.f);

		//Sanity checking / Can we actually shoot through?
		if (current_distance > penetration_distance && weapon_data->flPenetration > 0.f || enter_surf_penetration_modifier < 0.1f)
			break;

		//Calling HandleBulletPenetration here reduces our penetrationCounter, and if it returns true, we can't shoot through it.
		if (!handle_bullet_penetration(local, weapon_data, enter_trace, eye_position, direction, possible_hits_remaining, current_damage, penetration_power, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
			break;
	}
	return false;
}

float peeking_fakelag_autowall::can_hit(Vector& end_pos, C_CSPlayer* local) const
{
	if (!local)
		return 0;

	Vector angles, direction;
	const auto tmp = end_pos - local->GetEyePos();

	auto max_range = tmp.Length();

	Math::VectorAngles(tmp, angles);
	Math::AngleVectors(angles, &direction);
	direction.NormalizeInPlace();

	auto current_damage = 0.f;
	auto possible_hits_remaining = 4;
	if (fire_bullet(local, local->GetActiveWeapon(), max_range, direction, current_damage, possible_hits_remaining))
		return current_damage;

	return 0.f; //That wall is just a bit too thick buddy
}

bool fake_lag::adaptive_fl(C_CSPlayer* local, bool& send_packet)
{
	if (!local)
		return false;

	const auto sweet_spot = (g::previous_position - local->GetAbsOrigin()).Length() > 64.f;
	const auto is_moving = local->GetVelocity().Length() > 0 && !(local->GetFlags() & FL_ONGROUND) || INPUTSYSTEM->IsButtonDown(KEY_SPACE) && MiscVars::get().g_bBunnyHop; //local->GetVelocity().Length2D() > 50.f;

	if (!is_moving) //not moving, return false so that we can do our AA
		return false;

	if (!sweet_spot && CLIENTSTATE->net_channel->m_nChokedPackets < MiscVars::get().g_iFakeLagAmount)
		send_packet = false;

	return true;
}

bool fake_lag::static_fl(bool& send_packet)
{
	send_packet = !(CLIENTSTATE->net_channel->m_nChokedPackets < MiscVars::get().g_iFakeLagAmount);

	return true;
}

bool is_valid_target(C_CSPlayer* ent, C_CSPlayer* local)
{
	if (!local)
		return false;
	if (!ent)
		return false;
	if (ent == local)
		return false;
	if (!ent->IsPlayer())
		return false;
	if (g::plist[ent->EntIndex()].corrections.bWhiteListed)
		return false;
	if (!ent->IsAlive())
		return false;
	if (ent->GetTeam() == local->GetTeam())
		return false;
	if (ent->GetTeam() != 2 && ent->GetTeam() != 3)
		return false;
	if (ent->GetGunGameImmunity())
		return false;

	return true;
}

bool fake_lag::peeking_fl(C_CSPlayer* local, bool& send_packet)
{
	/*
	 * may be easier to just check if entity can hit head in like 7 ticks
	 * make sure we leave enough ticks to stop and become accurate so we can shoot
	 * and choke that amount of ticks until aimbot is true or we hit max choked ticks
	 */

	static auto in_peeking_fl = false;

	if (!local)
		return false;

	C_CSPlayer* entity = nullptr;

	float best_fov = 999.f;
	for (int i = 0; i < 64; i++)
	{
		if (i == ENGINE->GetLocalPlayer())
			continue;

		const auto p_entity = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));

		if (!is_valid_target(p_entity, local))
			continue;

		const auto fov = Math::GetFov(QAngle(0, Math::CalcAngle(local->GetEyePos(), p_entity->GetEyePos()).y, 0), g::cmd->viewangles);
		if (fov < best_fov)
		{
			entity = p_entity;
			best_fov = fov;
		}
	}

	if (!entity)
		return false;

	const auto extrap_amount = MiscVars::get().g_iFakeLagAmount - CLIENTSTATE->chokedcommands;
	auto pos = Utils::GetHitboxPosition(local, HITBOX_HEAD);
	const auto velocity = local->GetVelocity();
	auto extrapolated_pos = pos + velocity * (GLOBALS->interval_per_tick * extrap_amount);

	const auto extrap_dmg = peeking_fakelag_autowall::get().can_hit(extrapolated_pos, entity);
	const auto dmg = peeking_fakelag_autowall::get().can_hit(pos, entity);

	if ((extrap_dmg && !dmg) || (in_peeking_fl && extrap_dmg)) //if they can hit our extrapolated position but not our current, we need to start lagging
	{
		send_packet = false;
		in_peeking_fl = true;
		return true;
	}

	send_packet = true;
	in_peeking_fl = false;
	return false;
}

bool fake_lag::create_move(bool & send_packet)
{
	const auto local = C_CSPlayer::GetLocalPlayer();

	if (!local)
		return false;

	if (!local->IsAlive())
		return false;

	if (g::b_in_fake_walk)
		return true;

	if (MiscVars::get().g_nFakeLag == FLAG_OFF && !(MiscVars::get().g_bFakeLagOnKey && INPUTSYSTEM->IsButtonDown(MiscVars::get().g_kLagKey)))
		return false;

	if (MiscVars::get().g_bFakeLagOnKey && INPUTSYSTEM->IsButtonDown(MiscVars::get().g_kLagKey))
	{
		if (g::aimbotting)
		{
			send_packet = true;
			return false;
		}

		send_packet = false;
		return true;
	}

	if (MiscVars::get().g_bFakelagOnlyOnMove)
	{
		const auto is_moving = local->GetVelocity().Length() > 10.0f || !(local->GetFlags() & FL_ONGROUND);

		if (!is_moving)
			return false;
	}

	if (MiscVars::get().g_bFakelagOnLand)
	{
		const auto is_moving = local->GetVelocity().Length() > 1.0f && local->GetFlags() & FL_ONGROUND && INPUTSYSTEM->IsButtonDown(KEY_SPACE);

		if (is_moving)
		{
			send_packet = false;
			return true;
		}
	}

	if (CLIENTSTATE->net_channel->m_nChokedPackets >= 15)
		return true;

	if (MiscVars::get().g_nFakeLag == FLAG_NORMAL)
		return static_fl(send_packet);

	if (MiscVars::get().g_nFakeLag == FLAG_ADAPTIVE)
		return adaptive_fl(local, send_packet);

	if (MiscVars::get().g_nFakeLag == FLAG_ONPEEK)
		return peeking_fl(local, send_packet);

	return false;
}
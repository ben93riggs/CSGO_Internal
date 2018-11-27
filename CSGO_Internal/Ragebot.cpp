#include "Ragebot.h"
#include "Globals.h"
#include "Autowall.h"
#include "Config.h" 
#include "Accuracy.h"
#include <future>

std::vector<std::vector<CSGOHitboxID>> vec_hitgroup_hitboxes = {
	{ HITBOX_HEAD, HITBOX_NECK }, //head
	{ HITBOX_PELVIS, HITBOX_BELLY, HITBOX_THORAX, HITBOX_LOWER_CHEST, HITBOX_UPPER_CHEST, }, //chest
	{ HITBOX_RIGHT_UPPER_ARM, HITBOX_LEFT_UPPER_ARM }, //upper arms
	{ HITBOX_RIGHT_FOREARM, HITBOX_LEFT_FOREARM }, //lower arms
	{ HITBOX_RIGHT_THIGH, HITBOX_LEFT_THIGH }, // upper legs
	{ HITBOX_RIGHT_CALF, HITBOX_LEFT_CALF }, // lower legs
	{ HITBOX_RIGHT_HAND, HITBOX_LEFT_HAND }, //hands
	{ HITBOX_RIGHT_FOOT, HITBOX_LEFT_FOOT } //feet
};

void RageBot::HitScan::add_hitbox(CSGOHitboxID box)
{
	//if hitbox is not selected
	auto found_hitbox = false;
	auto iterator = -1; //start at -1 so we can iterate at the start of the function (for clarity)
	for (auto& hitgroup : vec_hitgroup_hitboxes)
	{
		iterator++;

		if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.HitgroupSelections[iterator]) //if this hitgroup is not selected, skip it
			continue;

		for (auto& hitbox : hitgroup)
		{
			if (hitbox == box)
			{
				found_hitbox = true;
				break;
			}
		}
		if (found_hitbox)
			break;
	}

	if (!found_hitbox)
		return;

	//if hitscan is off and the iterated hitbox is not the selected hitbox
	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bHitscan && box != WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_iHitbox)
		return;

	//if hitbox is already in the list
	for (auto& hitbox : boxes)
	{
		if (hitbox.hitbox == box)
			return;
	}

	boxes.emplace_back(box);
}

void RageBot::HitScan::clear_hitboxes()
{
	boxes.clear();
}

void RageBot::HitScan::fill_hitboxes(const bool blacklist_head)
{
	if (!blacklist_head)
	{
		add_hitbox(HITBOX_HEAD);
		add_hitbox(HITBOX_NECK);
	}
	add_hitbox(HITBOX_PELVIS);
	add_hitbox(HITBOX_BELLY);
	add_hitbox(HITBOX_THORAX);
	add_hitbox(HITBOX_RIGHT_FOOT);
	add_hitbox(HITBOX_LEFT_FOOT);
	add_hitbox(HITBOX_LOWER_CHEST);
	add_hitbox(HITBOX_UPPER_CHEST);
	add_hitbox(HITBOX_RIGHT_THIGH);
	add_hitbox(HITBOX_LEFT_THIGH);
	add_hitbox(HITBOX_RIGHT_CALF);
	add_hitbox(HITBOX_LEFT_CALF);
	add_hitbox(HITBOX_RIGHT_HAND);
	add_hitbox(HITBOX_LEFT_HAND);
	add_hitbox(HITBOX_RIGHT_UPPER_ARM);
	add_hitbox(HITBOX_RIGHT_FOREARM);
	add_hitbox(HITBOX_LEFT_UPPER_ARM);
	add_hitbox(HITBOX_LEFT_FOREARM);
}

void Accelerate(C_CSPlayer *player, Vector &wishdir, float wishspeed, float accel, Vector &outVel)
{
	// See if we are changing direction a bit
	float currentspeed = outVel.Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	float addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
		return;

	// Determine amount of accleration.
	float accelspeed = accel * GLOBALS->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	for (int i = 0; i < 3; i++)
		outVel[i] += accelspeed * wishdir[i];
}

void RageBot::DoAutoStop(C_CSPlayer* local) const
{
	if (!local)
		return;

	CBaseCombatWeapon* weapon = local->GetActiveWeapon();

	if (!weapon)
		return;

	const CCSWeaponInfo* weapon_info = weapon->GetCSWpnData();

	if (!weapon_info)
		return;

	//if (( ( WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bHitchance && weapon->GetId() == weapon_ssg08 )
	//      || weapon->GetId() == weapon_awp || weapon->GetId() == weapon_scar20 || weapon->GetId() == weapon_g3sg1) && !local->bCanFire())
	//	return;
	
	if (local->IsInReload())
		return;

	if (!(local->GetFlags() & FL_ONGROUND))
		return;

	Interface->MoveHelper()->SetHost(local);
	float speed = local->GetVelocity().Length2D();
	float want_speed = weapon_info->flMaxSpeed * .25f; //weapons are perfectly accurate at maxspeed / 3

	if (speed <= want_speed)
		return;
	
	float server_time = local->GetTickBase() * GLOBALS->interval_per_tick;
	float want_time = weapon->NextPrimaryAttack();
	int ticks_needed = max(1, (int)floor((want_time - server_time) / GLOBALS->interval_per_tick));
	
	float delta_speed = fabs(want_speed - speed);

	float wish_speed = (CVAR->FindVar(XorStr("sv_accelerate"))->GetFloat() * GLOBALS->frametime * local->m_surfaceFriction()) / delta_speed; // from sdk

	Interface->MoveHelper()->SetHost(nullptr);
	
	QAngle direction;
	Math::VectorAngles(g::velocity_unpredicted, direction);

	direction.y = g::cmd->viewangles.y - direction.y;

	Vector negated_direction = direction.forward() * (-wish_speed / ticks_needed);


	g::cmd->forwardmove = negated_direction.x;
	g::cmd->sidemove = negated_direction.y;

}

void RageBot::DoAutoDuck(C_CSPlayer* local)
{
	if (!local)
		return;

	CBaseCombatWeapon* weapon = local->GetActiveWeapon();

	if (!weapon)
		return;

	if ((WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bHitchance && weapon->GetId() == weapon_ssg08 || weapon->GetId() == weapon_awp) && !local->bCanFire())
				return;

	g::cmd->buttons |= IN_DUCK;
}

bool RageBot::IsValidTarget(C_CSPlayer* ent, C_CSPlayer* pLocal)
{
	if (!pLocal)
		return false;
	if (!ent)
		return false;
	if (ent == pLocal)
		return false;
	if (!ent->IsPlayer())
		return false;
	if (g::plist[ent->EntIndex()].corrections.bWhiteListed)
		return false;
	if (!ent->IsAlive())
		return false;
	if (ent->IsDormant())
		return false;
	if (ent->GetTeam() == pLocal->GetTeam())
		return false;
	if (ent->GetTeam() != 2 && ent->GetTeam() != 3)
		return false;
	if (ent->GetGunGameImmunity())
		return false;

	return true;
}

bool RageBot::InSelectedHitGroup(int hitgroup)
{
	switch (hitgroup)
	{
		case HITGROUP_HEAD:
			return WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.HitgroupSelections[0];
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
			return WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.HitgroupSelections[1];
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.HitgroupSelections[2] ||
				   WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.HitgroupSelections[3] ||
				   WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.HitgroupSelections[6];
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			return WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.HitgroupSelections[4] ||
				   WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.HitgroupSelections[5] ||
				   WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.HitgroupSelections[7];
		default: return false;
	}
}

bool RageBot::Hitchance(C_CSPlayer* pLocal, QAngle& angle) const
//second parameter is the angle we want to check against. in ragebot case it is always the angle to the best target (not current viewangles)
{
	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bHitchance)
		return true;

	Vector forward, right, up;

	constexpr auto max_traces = 150;
	int needed_hits = static_cast<int>(static_cast<float>(max_traces) * (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_flHitchanceValue / 100.f));

	Math::AngleVectors(angle, forward, right, up);

	if (!pLocal)
		return false;

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return false;

	const CCSWeaponInfo* weapon_info = weapon->GetCSWpnData();

	if (!weapon_info)
		return false;

	weapon->UpdateAccuracyPenalty();

	/*float weaponspread = weapon->GetInaccuracy() + weapon->GetSpread();

	if (weaponspread > 1.0f)
		weaponspread = 1.0f;

	weaponspread = (1.0f - weaponspread);*/

	int total_hits = 0;
	//int needed_hits = static_cast<int>((max_traces * 0.9f * weaponspread) * (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_flHitchanceValue / 100.f));

	auto eyes = pLocal->GetEyePos();
	auto flRange = weapon_info->flRange;

	for (int i = 0; i < max_traces; i++) 
	{
		Math::RandomSeed((i & 0xFF) + 1);

		constexpr float pi = M_PI;
		constexpr float pi2 = pi * 2.f;

		float fRand1 = Math::RandomFloat(0.f, 1.f);
		float fRandPi1 = Math::RandomFloat(0.f, pi2);
		float fRand2 = Math::RandomFloat(0.f, 1.f);
		float fRandPi2 = Math::RandomFloat(0.f, pi2);

		int m_iItemDefinitionIndex = weapon->GetItemDefinitionIndex();
		float m_flRecoilIndex = weapon->GetRecoilIndex();

		if (m_iItemDefinitionIndex == weapon_revolver)
		{
			if (g::cmd->buttons & IN_ATTACK2)
			{
				fRand1 = 1.f - fRand1 * fRand1;
				fRand2 = 1.f - fRand2 * fRand2;
			}
		}
		else if (m_iItemDefinitionIndex == weapon_negev && m_flRecoilIndex < 3.f)
		{
			for (int j = 3; j > m_flRecoilIndex; --j)
			{
				fRand1 *= fRand1;
				fRand2 *= fRand2;
			}

			fRand1 = 1.f - fRand1;
			fRand2 = 1.f - fRand2;
		}

		float fRandInaccuracy = fRand1 * weapon->GetInaccuracy();
		float fRandSpread = fRand2 * weapon->GetSpread();

		float fSpreadX = cos(fRandPi1) * fRandInaccuracy + cos(fRandPi2) * fRandSpread;
		float fSpreadY = sin(fRandPi1) * fRandInaccuracy + sin(fRandPi2) * fRandSpread;

		auto viewSpreadForward = (forward + (right * fSpreadX) + (up * fSpreadY)).Normalized();

		Vector viewAnglesSpread;
		Math::VectorAngles(viewSpreadForward, viewAnglesSpread);
		Math::GUWOPNormalize(viewAnglesSpread);
		

		Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, &viewForward);

		viewForward = eyes + (viewForward * flRange);

		trace_t tr;
		Ray_t ray;
		ray.Init(eyes, viewForward);

		ENGINETRACE->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, besttarget, &tr);

		if (tr.m_pEnt == besttarget)
			total_hits++;

		if (total_hits >= needed_hits)
			return true;

		if (needed_hits > (max_traces - i + total_hits))
			return false;
	}

	return false;
}

bool RageBot::Hitchance(C_CSPlayer* pLocal, C_CSPlayer* pEnt, QAngle& angle) const
//second parameter is the angle we want to check against. in ragebot case it is always the angle to the best target (not current viewangles)
{
	//DEBUGOVERLAY->ClearAllOverlays();

	Vector forward, right, up;

	constexpr auto max_traces = 150;

	Math::AngleVectors(angle, forward, right, up);

	if (!pLocal)
		return false;

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return false;

	const CCSWeaponInfo* weapon_info = weapon->GetCSWpnData();

	if (!weapon_info)
		return false;

	weapon->UpdateAccuracyPenalty();

	float weaponspread = weapon->GetInaccuracy() + weapon->GetSpread();

	if (weaponspread > 1.0f)
		weaponspread = 1.0f;

	weaponspread = (1.0f - weaponspread);

	int total_hits = 0;
	int needed_hits = static_cast<int>((max_traces * 0.9f * weaponspread) * (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_flHitchanceValue / 100.f));

	auto eyes = pLocal->GetEyePos();
	auto flRange = weapon_info->flRange;

	for (int i = 0; i < max_traces; i++)
	{
		Math::RandomSeed((i & 0xFF) + 1);

		constexpr float pi = M_PI;
		constexpr float pi2 = pi * 2.f;

		float fRand1 = Math::RandomFloat(0.f, 1.f);
		float fRandPi1 = Math::RandomFloat(0.f, pi2);
		float fRand2 = Math::RandomFloat(0.f, 1.f);
		float fRandPi2 = Math::RandomFloat(0.f, pi2);

		int m_iItemDefinitionIndex = weapon->GetItemDefinitionIndex();
		float m_flRecoilIndex = weapon->GetRecoilIndex();

		if (m_iItemDefinitionIndex == weapon_revolver)
		{
			if (g::cmd->buttons & IN_ATTACK2)
			{
				fRand1 = 1.f - fRand1 * fRand1;
				fRand2 = 1.f - fRand2 * fRand2;
			}
		}
		else if (m_iItemDefinitionIndex == weapon_negev && m_flRecoilIndex < 3.f)
		{
			for (int j = 3; j > m_flRecoilIndex; --j)
			{
				fRand1 *= fRand1;
				fRand2 *= fRand2;
			}

			fRand1 = 1.f - fRand1;
			fRand2 = 1.f - fRand2;
		}

		float fRandInaccuracy = fRand1 * weapon->GetInaccuracy();
		float fRandSpread = fRand2 * weapon->GetSpread();

		float fSpreadX = cos(fRandPi1) * fRandInaccuracy + cos(fRandPi2) * fRandSpread;
		float fSpreadY = sin(fRandPi1) * fRandInaccuracy + sin(fRandPi2) * fRandSpread;

		auto viewSpreadForward = (forward + (right * fSpreadX) + (up * fSpreadY)).Normalized();

		Vector viewAnglesSpread;
		Math::VectorAngles(viewSpreadForward, viewAnglesSpread);
		Math::GUWOPNormalize(viewAnglesSpread);

		Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, &viewForward);

		viewForward = eyes + (viewForward * flRange);

		trace_t tr;
		Ray_t ray;
		ray.Init(eyes, viewForward);
		
		ENGINETRACE->ClipRayToEntity(ray, MASK_SHOT, pEnt, &tr);
		if (tr.m_pEnt == pEnt)
		{
			DEBUGOVERLAY->AddLineOverlay(eyes, tr.endpos, 0, 255, 0, false, 0.25f);
			total_hits++;
		}
		else
		{
			DEBUGOVERLAY->AddLineOverlay(eyes, tr.endpos, 255, 0, 0, false, 0.25f);
		}

		if (total_hits >= needed_hits)
			return true;

		if (needed_hits > (max_traces - i + total_hits))
			return false;
	}

	return false;
}

bool RageBot::Aimstep(QAngle& src, QAngle& dst, QAngle& out, int steps = 34)
{
	QAngle delta_angle = (dst - src);
	Math::GUWOPNormalize(delta_angle);

	bool x_finished = false;
	bool y_finished = false;

	if (delta_angle.x > steps)
		src.x += steps;

	else if (delta_angle.x < -steps)
		src.x -= steps;

	else
	{
		x_finished = true;
		src.x = dst.x;
	}

	if (delta_angle.y > steps)
		src.y += steps;

	else if (delta_angle.y < -steps)
		src.y -= steps;

	else
	{
		y_finished = true;
		src.y = dst.y;
	}

	Math::GUWOPNormalize(src);

	out = src;

	return x_finished && y_finished;
}

//returns best damage in hitbox object passed to it, also writes bestpoint (aimspot) to the hitbox object
bool RageBot::HitScan::MultiPointHitbox(C_CSPlayer* ent, Hitbox& _hitbox, C_CSPlayer* pLocal, matrix3x4_t* matrix)
{
	if (!pLocal)
		return false;

	if (!ent)
		return false;

	_hitbox.bestpoint.Reset();

	studiohdr_t* hdr = MODELINFO->GetStudioModel(ent->GetModel());

	if (!hdr)
		return false;

	mstudiohitboxset_t* set = hdr->GetHitboxSet(ent->GetPlayerHitboxSet());

	if (!set)
		return false;

	mstudiobbox_t* hitbox = set->pHitbox(_hitbox.hitbox);

	if (!hitbox)
		return false;

	std::vector<Vector> points;
	points.push_back((hitbox->bbmin + hitbox->bbmax) * .5f); //center

	if (globals::get().closest_to_crosshair == ent)
	{
		float radius = hitbox->radius;
		Vector bbmin = hitbox->bbmin,
			   bbmax = hitbox->bbmax;

		if (hitbox->radius != -1.f)
		{
			bbmin -= radius;
			bbmax += radius;
		}

		bbmin *= WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_flPointScale;
		bbmax *= WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_flPointScale;

		if (_hitbox.hitbox == HITBOX_HEAD)
			points.push_back((Vector(bbmax.x, bbmin.y, bbmin.z) + Vector(bbmax.x, bbmin.y, bbmax.z)) * .5f); //top/back (for down/backwards AA)

		points.push_back((Vector(bbmax.x, bbmin.y, bbmin.z) + Vector(bbmax.x, bbmax.y, bbmax.z)) * .5f); //top		
		points.push_back((Vector(bbmin.x, bbmin.y, bbmax.z) + Vector(bbmax.x, bbmax.y, bbmax.z)) * .5f); //right		
		points.push_back((Vector(bbmax.x, bbmin.y, bbmin.z) + Vector(bbmin.x, bbmax.y, bbmin.z)) * .5f); //left
	}

	for (auto& point : points)
	{
		Vector hitbox_point;
		Math::VectorTransform(point, matrix[hitbox->bone], hitbox_point);
		int temp_damage = CAutoWall::get().can_hit(hitbox_point, pLocal);

		if (!temp_damage)
			continue;

		if (temp_damage < WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_iMinimumDamage)
			continue;

		if (temp_damage > _hitbox.bestpoint.dmg)
		{
			_hitbox.bestpoint.dmg = temp_damage;
			_hitbox.bestpoint.point = hitbox_point;
		}

		if (_hitbox.bestpoint.dmg > ent->GetHealth())
			return _hitbox.bestpoint.dmg;

	}

	if (!_hitbox.bestpoint.dmg)
		return false;

	return _hitbox.bestpoint.dmg;
}

bool RageBot::HitScan::Scan(C_CSPlayer* pEntity, C_CSPlayer* pLocal, Vector& position_out, float& dmg_out)
{
	clear_hitboxes();

	const auto i = pEntity->EntIndex();
	
	if (g::plist[i].corrections.Bodyaim/*WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bSmartHitscan || g::pList[i].corrections.Bodyaim || g::pList[i].corrections.bAimThisShot || */)
	{
		add_hitbox(HITBOX_PELVIS);
		add_hitbox(HITBOX_THORAX);
		add_hitbox(HITBOX_LOWER_CHEST);
		add_hitbox(HITBOX_THORAX);
		add_hitbox(HITBOX_UPPER_CHEST);
	}

	fill_hitboxes(MenuVars::get().g_bResolveYaw && Accuracy::get().Players[i].m_bShouldBaim && Accuracy::get().Players[i].m_bBacktrackedToLby == 0);

	CBoneAccessor* boneaccessor = pEntity->GetBoneAccessor();

	if (!boneaccessor)
		return false;

	Hitbox besthitbox;
	for (Hitbox& hitbox : boxes)
	{
		hitbox.bestpoint.Reset();

		//get hitbox data (best point (Vector), damage), if dmg = 0 return false
		if (!MultiPointHitbox(pEntity, hitbox, pLocal, boneaccessor->m_pBones))
			continue;

		//if we can kill with this shot, take the shot
		if (hitbox.bestpoint.dmg >= pEntity->GetHealth())
		{
			position_out = hitbox.bestpoint.point;
			dmg_out = hitbox.bestpoint.dmg;
			return true;
		}

		//if this hitbox is better than current best, set as best hitbox
		if (hitbox.bestpoint.dmg > besthitbox.bestpoint.dmg)
			besthitbox = hitbox;
	}

	if (besthitbox.bestpoint.dmg > 0)
	{
		position_out = besthitbox.bestpoint.point;
		dmg_out = besthitbox.bestpoint.dmg;
		return true;
	}

	return false;
}

float RageBot::FindTarget(C_CSPlayer* pLocal)
{
	float bestdamage = 0.f;
	besttarget = nullptr;
	g::besttarget = nullptr;
	bestindex = -1;
	aimangle.Init();

	for (int i = 0; i <= 64; i++)
	{
		//Skip the pLocal player
		if (i == ENGINE->GetLocalPlayer())
			continue;

		C_CSPlayer* pEntity = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));

		if (!IsValidTarget(pEntity, pLocal))
			continue;

		Vector aimspotpos;
		float damage;
		if (!HitScan::get().Scan(pEntity, pLocal, aimspotpos, damage))
			continue;

		if (!aimspotpos.IsValidAndNotZero())
			continue;

		Vector localpos = pLocal->GetEyePos();
		QAngle ent_angle = Math::CalcAngle(localpos, aimspotpos);

		if (damage >= pEntity->GetHealth())
		{
			bestdamage = damage;
			besttarget = pEntity;
			g::besttarget = pEntity;
			bestindex = i;
			aimangle = ent_angle;
			break;
		}
		
		if (damage > bestdamage)
		{
			bestdamage = damage;
			besttarget = pEntity;
			g::besttarget = pEntity;
			bestindex = i;
			aimangle = ent_angle;
		}
		
	}

	return bestdamage;
}

bool RageBot::AimKeyPressed()
{
	if (MenuVars::get().g_bMainWindow)
		return false;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAutoFire)
		return true;

	if (INPUTSYSTEM->IsButtonDown(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].menu.g_kAimKey))
		return true;

	if (INPUTSYSTEM->IsButtonDown(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].menu.g_kTriggerKey))
		return true;

	return false;
}

bool RageBot::Aim(C_CSPlayer* pLocal, bool& bRet, bool& bSendPacket, float dmg)
{
	if (!besttarget)
		return false;

	if (bestindex == -1)
		return false;

	CBaseCombatWeapon* pLocalWeapon = pLocal->GetActiveWeapon();

	if (!pLocalWeapon)
		return false;

	if (pLocalWeapon->GetWeaponType() == WEAPONTYPE_OTHER)
		return false;

	if (!AimKeyPressed())
		return false;

	if (!pLocal->IsInReload())
	{
		if (!Hitchance(pLocal, aimangle)) { // we only want to do these if hitchance is low enough

			if (pLocalWeapon->GetWeaponType() == WEAPONTYPE_SNIPER || pLocalWeapon->GetId() == weapon_sg553 || pLocalWeapon->GetId() == weapon_aug)
			{
				if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAutoScope)
				{
					if (pLocalWeapon->GetZoomLevel() == 0)
					{
						g::cmd->buttons |= IN_ATTACK2;
					}
				}
			}

			if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAutoDuck)
				DoAutoDuck(pLocal);

			if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAutoStop)
			{
				if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAutoStopOnKillShot || dmg >= besttarget->GetHealth())
					DoAutoStop(pLocal);
			}
			return false;
		}
	}
	else return false;

	if (!pLocal->bCanFire())
		return false;
	
	g::cmd->tick_count = TIME_TO_TICKS(besttarget->GetSimulationTime() + Accuracy::get().GetLerpTime());

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAimStep && !bFinished)
	{
		//aim @ target
		QAngle angNextAngle;
		QAngle aimangle_recoilcompensated = aimangle - pLocal->AimPunch() * 2.f;
		bFinished = Aimstep(LastAngle, aimangle_recoilcompensated, angNextAngle, 34);
		g::cmd->viewangles = angNextAngle;
		LastAngle = angNextAngle;
	}
	else
	{
		//aim @ target
		g::cmd->viewangles = aimangle - pLocal->AimPunch() * 2.f;;
	}

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_iSilentAim)
	{
		bRet = false;

		if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAimStep && !bFinished)
			ENGINE->SetViewAngles(g::cmd->viewangles);
	}

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_iSilentAim == 2 && pLocalWeapon->GetItemDefinitionIndex() != weapon_revolver)
		bSendPacket = false;

	return true;
}

void RageBot::AutoFire(C_CSPlayer* pLocal, bool& bSendPacket) const
{
	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAutoFire)
		return;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAimStep && !bFinished)
		return;

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return;
	
	if (weapon->GetItemDefinitionIndex() == weapon_revolver)
	{
		if (pLocal->IsInReload() || g::cmd->buttons & IN_RELOAD)
			return;

		static float flLastRevolverTime = 0.f;

		if (GLOBALS->curtime < weapon->GetPostponeFireReadyTime() || GLOBALS->curtime - flLastRevolverTime > 0.2f)
		{
			flLastRevolverTime = GLOBALS->curtime;
			g::cmd->buttons |= IN_ATTACK;
			bSendPacket = true;
			g::aimbotting = false;
			return;
		}
	}

	if (g::aimbotting) //if aimbot is active skip the revolver check and aimbot that fool
	{
		if (MenuVars::get().g_iCurrentAimbotMode == 2 && weapon->GetItemDefinitionIndex() == weapon_revolver)
			g::cmd->buttons |= IN_ATTACK2;
		else
			g::cmd->buttons |= IN_ATTACK;
	}
}

void RageBot::CreateMove(bool& bRet, bool& bSendPacket)
{
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	Accuracy::get().Backtrack();

	LastAngle = g::cmd->viewangles;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAimStep)
		bFinished = false;

	const auto dmg = FindTarget(pLocal); 

	g::aimbotting = Aim(pLocal, bRet, bSendPacket, dmg);

	AutoFire(pLocal, bSendPacket);
	
	if(g::besttarget)
		Accuracy::get().AddShot(pLocal, dynamic_cast<C_CSPlayer*>(g::besttarget));
	Accuracy::get().RestorePlayers();
}
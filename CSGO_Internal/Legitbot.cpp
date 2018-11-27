#pragma once
#include "Legitbot.h"
#include "Globals.h"

#include "Config.h"
#include "Accuracy.h"


bool bShouldntAim = false;
bool bInSticky = false;
C_CSPlayer * oldtarget;
C_CSPlayer * besttarget;
float m_oldforward;
float m_oldsidemove;
int shotsFired;
int lastShotsFired;

enum AimTargetType : int
{
	FOV,
	REAL_DISTANCE,
	BACKTRACKING
	//DISTANCE
};

enum SmoothType : int
{
	CONSTANT,
	FAST_END,
	SLOW_END,
};

bool Legitbot::AimKeyPressed(C_CSPlayer* pLocal)
{
	if (MenuVars::get().g_bMainWindow)
		return false;

	/*if (!pLocal->bCanFire())
		return false;*/

	//Move the view angle to the aimangles of target (if aimbot key is pressed)
	if ((INPUTSYSTEM->IsButtonDown(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].menu.g_kAimKey)
		|| INPUTSYSTEM->IsButtonDown(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].menu.g_kTriggerKey)
		|| WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bAutoAim)
		&& (pLocal->bCanFire() || !WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bPSilent)
		&& WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_iAimbotStyle != 2)
		return true;

	return false;
}

void Legitbot::DoAutoStop(C_CSPlayer* local)
{
	if (!local)
		return;

	/*CBaseCombatWeapon* weapon = local->GetActiveWeapon();

	if (!weapon)
		return;*/

	/*if ((WeaponConfigVars::get().WeaponConfig[G::weaponconfig].triggerbot.g_bHitChance && weapon->GetId() == weapon_ssg08 || weapon->GetId() == weapon_awp) && !local->bCanFire())
		return;*/

	g::cmd->forwardmove = 0;
	g::cmd->sidemove = 0;
}

void Legitbot::DoAutoDuck(C_CSPlayer* local)
{
	if (!local)
		return;

	CBaseCombatWeapon* weapon = local->GetActiveWeapon();

	if (!weapon)
		return;

	if ((WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_bHitChance && weapon->GetId() == weapon_ssg08 || weapon->GetId() == weapon_awp) && !local->bCanFire())
		return;

	g::cmd->buttons |= IN_DUCK;
}

bool Legitbot::GetBestBoneTarget(C_CSPlayer* pLocal, C_CSPlayer* pEntity, Vector& vec_out)
{
	std::vector<int> HitBoxesToScan;
	// Get the hitboxes to scan

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bHitscanEnabled)
	{
		// heaps ++(just all the random stuff)
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_HEAD));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_NECK));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_PELVIS));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_BELLY));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_THORAX));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_LOWER_CHEST));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_UPPER_CHEST));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_RIGHT_THIGH));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_LEFT_THIGH));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_RIGHT_CALF));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_LEFT_CALF));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_RIGHT_FOOT));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_LEFT_FOOT));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_RIGHT_HAND));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_LEFT_HAND));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_RIGHT_UPPER_ARM));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_RIGHT_FOREARM));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_LEFT_UPPER_ARM));
		HitBoxesToScan.push_back(static_cast<int>(HITBOX_LEFT_FOREARM));
	}
	else
	{
		Vector pos = Utils::GetHitboxPosition(pEntity, CSGOHitboxID(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_iHitbox));
		if (pEntity->IsVisible(pos))
		{
			vec_out = pos;

			if (vec_out.IsValid() && !vec_out.IsZero())
				return true;
		}
	}

	// check hits
	for (auto HitBoxID : HitBoxesToScan)
	{
		Vector pos = Utils::GetHitboxPosition(pEntity, CSGOHitboxID(HitBoxID));
		if (pEntity->IsVisible(pos))
		{
			vec_out = pos;

			if (vec_out.IsValid() && !vec_out.IsZero())
				return true;
		}
	}

	return false;
}

float Legitbot::GetRealDistanceFOV(float distance, QAngle angle, CUserCmd* cmd) const
{
	Vector aimingAt;
	Math::AngleVectors(cmd->viewangles, &aimingAt);
	aimingAt *= distance;

	Vector aimAt;
	Math::AngleVectors(angle, &aimAt);
	aimAt *= distance;

	return aimingAt.DistTo(aimAt);
}

bool Legitbot::Hitchance(C_CSPlayer* pLocal, QAngle& angle) const
//second parameter is the angle we want to check against
{
	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_bHitChance)
		return true;

	Vector forward, right, up;

	constexpr auto max_traces = 150;

	Math::AngleVectors(angle, forward, right, up);

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
	int needed_hits = static_cast<int>((max_traces * 0.9f * weaponspread) * (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].triggerbot.g_flHitchancePercentage / 100.f));

	auto eyes = pLocal->GetEyePos();
	auto flRange = weapon_info->flRange;

	for (int i = 0; i < max_traces; i++)
	{
		Math::RandomSeed((i & 0xFF) + 1);

		//constexpr auto pi = M_PI;

		float fRand1 = Math::RandomFloat(0.f, 1.f);
		float fRandPi1 = Math::RandomFloat(0.f, 2.f * M_PI);
		float fRand2 = Math::RandomFloat(0.f, 1.f);
		float fRandPi2 = Math::RandomFloat(0.f, 2.f * M_PI);

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
			for (auto iLoop = 3; iLoop > m_flRecoilIndex; --iLoop)
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
		//Utils::GUWOPNormalize(viewForward);

		viewForward = eyes + (viewForward * flRange);

		trace_t tr;
		Ray_t ray;
		ray.Init(eyes, viewForward);

		ENGINETRACE->ClipRayToEntity(ray, MASK_SHOT, besttarget, &tr);

		if (tr.m_pEnt == besttarget)
			total_hits++;

		if (total_hits >= needed_hits)
			return true;

		if ((max_traces - i + total_hits) < needed_hits)
			return false;
	}

	return false;
}

void Legitbot::ApplyErrorToAngle(QAngle* angles, float margin)
{
	QAngle error;
	error.Random(-1.0f, 1.0f);
	error *= margin;
	angles->operator+=(error);
}

//void Salt(float& smooth)
//{
//	float sine = sin(GLOBALS->tickcount);
//	float salt = sine * WeaponConfigVars::get().WeaponConfig[G::weaponconfig].legitbot.g_flSaltValue; //goes from 0.f to whatever smooth is set to
//	float oval = smooth + salt;
//	smooth *= oval;
//}

void Legitbot::Smooth(C_CSPlayer* player, QAngle& angle, CUserCmd* cmd) const
{
	if (!player)
		return;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bPSilent)
		return;

	QAngle viewAngles = QAngle(0.f, 0.f, 0.f);
	ENGINE->GetViewAngles(viewAngles);

	QAngle delta = angle - viewAngles;
	Math::GUWOPNormalize(delta);

	float smoothx = powf(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_flSmoothX, 0.4f);
	float smoothy = powf(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_flSmoothY, 0.4f);

	#undef min
	smoothx = std::min(0.99f, smoothx);
	smoothy = std::min(0.99f, smoothy);

	/*if (WeaponConfigVars::get().WeaponConfig[G::weaponconfig].legitbot.g_bSalt)
	{
		Salt(smoothx);
		Salt(smoothy);
	}*/
		
	QAngle toChange = QAngle();

	auto type = static_cast<int>(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_iSmoothType);

	if (type == (int)SmoothType::SLOW_END)
	{
		toChange.x = delta.x - delta.x * smoothx;
		toChange.y = delta.y - delta.y * smoothy;
	}
	else if (type == (int)SmoothType::CONSTANT || type == (int)SmoothType::FAST_END)
	{
		float coeffx = (1.0f - smoothx) / delta.Length() * 4.f;
		float coeffy = (1.0f - smoothy) / delta.Length() * 4.f;

		if (type == (int)SmoothType::FAST_END)
		{
			coeffx = powf(coeffx, 2.f) * 10.f;
			coeffy = powf(coeffy, 2.f) * 10.f;
		}

		coeffx = std::min(1.f, coeffx);
		coeffy = std::min(1.f, coeffy);
		toChange.x = delta.x * coeffx;
		toChange.y = delta.y * coeffy;
	}

	angle = viewAngles + toChange;
}

bool Legitbot::GetBestTarget(C_CSPlayer* pLocal, C_CSPlayer*& target_out) const
{
	if (!pLocal)
		return false;

	if (!pLocal->IsAlive())
		return false;

	if (g::cmd->buttons & IN_USE)
		return false;

	//if (WeaponConfigVars::get().WeaponConfig[G::weaponconfig].legitbot.g_bStickToTarget && besttarget && besttarget->IsAlive() && (INPUTSYSTEM->IsButtonDown(WeaponConfigVars::get().WeaponConfig[G::weaponconfig].menu.g_kAimKey) || INPUTSYSTEM->IsButtonDown(WeaponConfigVars::get().WeaponConfig[G::weaponconfig].menu.g_kTriggerKey) || WeaponConfigVars::get().WeaponConfig[G::weaponconfig].triggerbot.g_bAutoshoot) && (pLocal->bCanFire() || !WeaponConfigVars::get().WeaponConfig[G::weaponconfig].legitbot.g_bPSilent))
	//{
	//	Vector aimspotpos;
	//	if (!GetBestBoneTarget(pLocal, besttarget, aimspotpos))
	//	{
	//		bShouldntAim = true;
	//		return false;
	//	}

	//	bShouldntAim = false;

	//	return true;
	//}

	float bestdistance = 99999;
	besttarget = nullptr;
	g::besttarget = nullptr;

	for (int i = 0; i <= 64; i++)
	{
		//Skip the local player
		if (i == ENGINE->GetLocalPlayer())
			continue;

		//Gets the entity by index
		C_CSPlayer * pEntity = static_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));

		if (!pEntity)
			continue; //Null check 

		if (pEntity->GetTeam() == pLocal->GetTeam())
			continue; //ignore teammates

		if (!pEntity->IsAlive() || pEntity->IsDormant() || pEntity->GetGunGameImmunity())
			continue; //Skip Dead and Dormant entities

		ClientClass* ent_client_class = pEntity->GetClientClass();

		if (!ent_client_class)
			continue;

		if (ent_client_class->m_ClassID != CCSPlayer)
			continue; //We only want to iterate over players. Make sure the ClassID is correct

        Vector aimspotpos;// = Utils::GetHitboxPosition(pEntity, NeckLower);
        GetBestBoneTarget(pLocal, pEntity, aimspotpos);
		Vector localpos = pLocal->GetEyePos();

		if (!aimspotpos.IsValid() || aimspotpos.IsZero())
		{
			bShouldntAim = true;
			g::aimbotting = false;
			continue;
		}

		QAngle viewAngles;
		ENGINE->GetViewAngles(viewAngles);

		float distance = aimspotpos.DistTo(localpos);
		float fov = Math::GetFov(viewAngles, Math::CalcAngle(localpos, aimspotpos));
		float real_distance = GetRealDistanceFOV(distance, Math::CalcAngle(localpos, aimspotpos), g::cmd);

		float value_to_compare;
		switch (static_cast<AimTargetType>(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_iAimbotStyle))
		{
		case FOV:
			value_to_compare = fov;
			break;
		//case AimTargetType::DISTANCE:
		//	value_to_compare = distance;
		//	break;
		case REAL_DISTANCE:
			value_to_compare = real_distance;
			break;
		default:
			value_to_compare = fov;
			break;
		}

		// If the difference between current and aimbot angle is better for this entity than the current best then it becomes the current best
		if (value_to_compare < bestdistance && value_to_compare <= WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_iFOV)
		{
			bestdistance = value_to_compare;
			g::besttarget = pEntity;
			besttarget = pEntity;
		}
	}

	bShouldntAim = false;

	target_out = besttarget;

	if (target_out)
		return true;

	return false;
}

bool Legitbot::AimAtPlayer(C_CSPlayer* pLocal, C_CSPlayer* target, bool& bRet, bool& bSendPacket) const
{
	if (!pLocal)
		return false;

	if (!target)
		return false;

	if (!target->IsAlive())
		return false;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bIgnoreJumpingEnemy && (target->GetFlags() & FL_ONGROUND) == 0)
		return false;

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return false;

	if (weapon->GetWeaponType() == WEAPONTYPE_OTHER)  //if player doesnt have knife/nades/c4/etc out
		return false;

	if (pLocal->IsInReload())
		return false;

	Vector localpos = pLocal->GetEyePos();

	Vector aimspotpos;
	if (!GetBestBoneTarget(pLocal, target, aimspotpos))
	{
		bShouldntAim = true;
		return false;
	}

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bSmokeCheck && Utils::LineGoesThroughSmoke(localpos, aimspotpos))
		return false;

	QAngle aimangle = Math::CalcAngle(localpos, aimspotpos);
	
	// Changes the aimangles by punch angles to compensate for recoil
	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bRCSEnabled && weapon->GetWeaponType() != WEAPONTYPE_SNIPER)
	{
		Vector punch = pLocal->AimPunch();
		aimangle.x -= punch.x * (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_flRCSPitchScale  * 0.02f);
		aimangle.y -= punch.y * (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_flRCSYawScale * 0.02f);
	}

	 //Move the view angle to the aimangles of target (if aimbot key is pressed)
	if ((INPUTSYSTEM->IsButtonDown(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].menu.g_kAimKey) || INPUTSYSTEM->IsButtonDown(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].menu.g_kTriggerKey) || WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bAutoAim) && (pLocal->bCanFire() || !WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bPSilent) && WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_iAimbotStyle != 2)
	{
		if (!bSendPacket)
			bSendPacket = true;

		if (!bRet)
			bRet = true;

		if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bPSilent)
		{
			bSendPacket = false;
			//G::bShouldSendNextPacket = true;
			bRet = false;
		}

		Math::Clamp(aimangle);

		//ApplyErrorToAngle(&aimangle, WeaponConfigVars::get().WeaponConfig[G::weaponconfig].legitbot.g_flErrorMargin);

		if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bPSilent)
			Smooth(target, aimangle, g::cmd);

		Math::Clamp(aimangle);
		g::cmd->viewangles = aimangle;

		if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bAutoStop && g::aimbotting && target)
			DoAutoStop(pLocal);

		if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bAutoDuck && g::aimbotting && target)
			DoAutoDuck(pLocal);

		return true;
	}
	
	//helper mode
	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_iAimbotStyle == 2)
	{
		Math::Clamp(aimangle);

		QAngle angle_delta = g::cmd->viewangles - aimangle;

		bool right = g::cmd->mousedx > 0;
		bool left = g::cmd->mousedx < 0;
		bool ch_right = angle_delta.y > 0;
		bool ch_left = angle_delta.y < 0;
		bool ch_down = angle_delta.x > 0;
		bool ch_up = angle_delta.x < 0;

		if (!(left && ch_left) && !(right && ch_right))
			return false;

		//correct yaw
		if (right && ch_right)
			g::cmd->viewangles.y -= WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_flSmoothY * .5f;
		else if (left && ch_left)
			g::cmd->viewangles.y += WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_flSmoothY * .5f;

		//correct pitch
		if (ch_up)
			g::cmd->viewangles.x -= WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_flSmoothX * .5f;
		else if (ch_down)
			g::cmd->viewangles.x -= WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_flSmoothX * .5f;

		Math::Clamp(g::cmd->viewangles);

		return true;
	}


	return false;
}

bool Legitbot::AimWithBacktrack(C_CSPlayer* pLocal, C_CSPlayer* target, bool& bRet, bool& bSendPacket)
{
	if (!pLocal)
		return false;

	if (!AimKeyPressed(pLocal))
		return false;

	if (!target)
		return false;

	if (!target->IsAlive())
		return false;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bIgnoreJumpingEnemy && (target->GetFlags() & FL_ONGROUND) == 0)
		return false;

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return false;

	if (weapon->GetWeaponType() == WEAPONTYPE_OTHER)  //if player doesnt have knife/nades/c4/etc out
		return false;

	if (pLocal->IsInReload())
		return false;

	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].resolver.g_bBackTracking)
		return false;

	if (Accuracy::get().Players[target->EntIndex()].m_LagRecord.size() <= 1)
		return false;

	Accuracy::get().BacktrackLegit(pLocal, target);

	g::cmd->tick_count = TIME_TO_TICKS(target->GetSimulationTime());

	Accuracy::get().RestorePlayers();
	return true;
}

void Legitbot::CreateMove(bool& bRet, bool& bSendPacket) const
{
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (!pLocal->IsAlive())
		return;

	C_CSPlayer* target = nullptr;
	GetBestTarget(pLocal, target);

	if (!bShouldntAim)
	{
		g::aimbotting = AimAtPlayer(pLocal, target, bRet, bSendPacket);
		if (g::aimbotting)
			AimWithBacktrack(pLocal, target, bRet, bSendPacket);//
	}
}

/*
 *
 *
 *
 *				*LEGIT BACKTRACKING*
 *	-SELECT BEST TARGET PER USUAL
 *	-IF MOUSE1 IS PRESSED, SCAN FOR TICK TO SET TO AND BACKTRACK TO THAT POINT
 *
 */
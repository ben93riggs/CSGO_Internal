#include "BHop.h"
#include "Globals.h"


enum LegitStrafeType
{
	AS_FORWARDS,
	AS_BACKWARDS,
	AS_LEFTSIDEWAYS,
	AS_RIGHTSIDEWAYS
};

void CorrectMovement(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove)
{
	// side/forward move correction
	float deltaView;
	float f1;
	float f2;

	if (vOldAngles.y < 0.f)
		f1 = 360.0f + vOldAngles.y;
	else
		f1 = vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
}

void BunnyHop::AutoHop(C_CSPlayer* pLocal)
{
	if (!MiscVars::get().g_bBunnyHop)
		return;

	if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	static bool bLastJumped = false;
	static bool bShouldFake = false;

	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		g::cmd->buttons |= IN_JUMP;
	}
	else if (g::cmd->buttons & IN_JUMP)
	{
		if (pLocal->GetFlags() & FL_ONGROUND)
		{
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			g::cmd->buttons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldFake = false;
	}
}

void BunnyHop::LegitStrafe(C_CSPlayer* pLocal)
{
	if (pLocal->GetFlags() & FL_ONGROUND)
		return;

	if (g::cmd->buttons & IN_FORWARD || g::cmd->buttons & IN_BACK || g::cmd->buttons & IN_MOVELEFT || g::cmd->buttons & IN_MOVERIGHT)
		return;

	if (g::cmd->mousedx <= 1 && g::cmd->mousedx >= -1)
		return;

	switch (MiscVars::get().g_iStrafeDirection)
	{
	case LegitStrafeType::AS_FORWARDS:
		g::cmd->sidemove = g::cmd->mousedx < 0.f ? -450.f : 450.f;
		break;
	case LegitStrafeType::AS_BACKWARDS:
		g::cmd->sidemove = g::cmd->mousedx < 0.f ? 450.f : -450.f;
		break;
	case LegitStrafeType::AS_LEFTSIDEWAYS:
		g::cmd->forwardmove = g::cmd->mousedx < 0.f ? -450.f : 450.f;
		break;
	case LegitStrafeType::AS_RIGHTSIDEWAYS:
		g::cmd->forwardmove = g::cmd->mousedx < 0.f ? 450.f : -450.f;
		break;
	default:
		break;
	}
}
/*
* get_move_angle
* Returns the movement angle from the current speed
*/
float get_move_angle(float speed)
{
	auto move_angle = RAD2DEG(asin(30.f / speed));

	if (!isfinite(move_angle) || move_angle > 90.f)
		move_angle = 90.f;
	else if (move_angle < 0.f)
		move_angle = 0.f;

	return move_angle;
}

void BunnyHop::RageStrafe(C_CSPlayer* pLocal)
{
	static bool leftRight;
	bool inMove = g::cmd->buttons & IN_FORWARD || g::cmd->buttons & IN_BACK || g::cmd->buttons & IN_MOVELEFT || g::cmd->buttons & IN_MOVERIGHT;

	if (g::cmd->buttons & IN_FORWARD && pLocal->GetVelocity().Length() <= 50.0f)
		g::cmd->forwardmove = 450.0f;

	//float yaw_change = get_move_angle(pLocal->GetVelocity().Length2D());

	float yaw_change = 0.0f;	
	
	if (pLocal->GetVelocity().Length() > 50.f)
		yaw_change = 30.0f * fabsf(30.0f / pLocal->GetVelocity().Length());

	CBaseCombatWeapon* activeWeapon = pLocal->GetActiveWeapon();

	if (activeWeapon && !activeWeapon->GetClip1() == 0 && g::cmd->buttons & IN_ATTACK)
		yaw_change = 0.0f;

	QAngle viewAngles;
	ENGINE->GetViewAngles(viewAngles);

	if (!(pLocal->GetFlags() & FL_ONGROUND) && !inMove)
	{
		if (leftRight || g::cmd->mousedx > 1)
		{
			viewAngles.y += yaw_change;
			g::cmd->sidemove = 450.0f;
		}
		else if (!leftRight || g::cmd->mousedx < 1)
		{
			viewAngles.y -= yaw_change;
			g::cmd->sidemove = -450.0f;
		}

		leftRight = !leftRight;
	}
	else if ((pLocal->GetFlags() & FL_ONGROUND) && !inMove && (g::cmd->buttons & IN_JUMP))
	{
		g::cmd->sidemove = 0;
		g::cmd->forwardmove = 450.f;
	}

	//Utils::NormalizeAngles(viewAngles);
	Math::Clamp(viewAngles);

	CorrectMovement(viewAngles, g::cmd, g::cmd->forwardmove, g::cmd->sidemove);

	//if (!Settings::AutoStrafe::silent)
	//G::UserCmd->viewangles = viewAngles;
}

void BunnyHop::AutoStrafe(C_CSPlayer* pLocal)
{
	if (!MiscVars::get().g_bAutoStrafe)
		return;

	if (MiscVars::get().g_bBunnyHop && INPUTSYSTEM->IsButtonDown(MenuVars::get().g_kCircleStrafeKey))
		return;

	if (pLocal->GetMoveType() == MOVETYPE_LADDER || pLocal->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	//do strafe based on selection
	if (MiscVars::get().g_bRageStrafe)
		RageStrafe(pLocal);
	else
		LegitStrafe(pLocal);
}

void BunnyHop::CircleStrafe(C_CSPlayer* pLocal) const
{
	if (!MiscVars::get().g_bCircleStrafe)
		return;

	if (!MiscVars::get().g_bRageStrafe)
		return;

	static float angle = 0.f;
	static int inv = -1;

	if (!INPUTSYSTEM->IsButtonDown(MenuVars::get().g_kCircleStrafeKey))
	{
		angle = g::cmd->viewangles.y - 180.f;
		return;
	}

	Ray_t left_trace;
	Ray_t right_trace;
	Ray_t normal_trace;
	CTraceFilter Filter;
	Filter.pSkip = pLocal;
	Vector EyePos = pLocal->GetOrigin();
	EyePos.z += 50.f;

	Vector vleft = Vector(0.f, angle + 30.f - 180.f, 0.f),
		   vright = Vector(0.f, angle - 30.f - 180.f, 0.f),
		   vnormal = Vector(0.f, angle - 180.f, 0.f),
			left, right, normal;
	Math::AngleVectors(vright, &right);
	Math::AngleVectors(vleft, &left);
	Math::AngleVectors(vnormal, &normal);

	left_trace.Init(EyePos, EyePos + left * 200.f, Vector(-16, -16, -16), Vector(16, 16, 16));
	right_trace.Init(EyePos, EyePos + right * 200.f, Vector(-16, -16, -16), Vector(16, 16, 16));
	normal_trace.Init(EyePos, EyePos + normal * 200.f, Vector(-16, -16, -16), Vector(16, 16, 16));

	trace_t result_left, result_right, result;
	//ENGINETRACE->TraceRay(left_trace, MASK_SOLID, &Filter, &result_left);
	ENGINETRACE->TraceRay(right_trace, MASK_SOLID, &Filter, &result_right);
	ENGINETRACE->TraceRay(normal_trace, MASK_SOLID, &Filter, &result);

	//If forward or right path is blocked, turn
	if ((result.fraction != 1.f && !result.m_pEnt) || (result_right.fraction != 1.f && !result_right.m_pEnt))
	{
		float fraction = min(result.fraction, result_right.fraction);
		inv = -1;
		angle -= (30.f + (60.f * (1.f - fraction)));
		g::cmd->sidemove = (-450 + (200.f * (1.f - fraction)));
	}
	//else if (result_left.fraction != 1.f && !result_left.m_pEnt) //if left path is blocked, turn
	//{
	//	inv = 1;
	//	angle += 30.f + (60.f * (1.f - result_left.fraction));
	//	G::UserCmd->sidemove = 250;
	//}
	else
	{
		float flAdd = (200.f / pLocal->GetVelocity().Length2D()) * 16.f;
		if (flAdd > 5.f) flAdd = 5.f;
		angle += flAdd * (float)inv;
		g::cmd->sidemove = 450.f * (float)inv;
	}

	g::cmd->viewangles.y = angle;
}

void BunnyHop::AirCrouch( C_CSPlayer *pLocal ) {
	if (!MiscVars::get().g_bAirCrouch)
		return;

	if (!pLocal)
		return;
	
	if (!(pLocal->GetFlags() & FL_ONGROUND))
		g::cmd->buttons |= IN_DUCK;
}


/*
* get_closest_plane
* Returns the closest plane
*/
bool get_closest_plane(Vector* plane, int &ticks)
{
	C_CSPlayer *local = C_CSPlayer::GetLocalPlayer();
	trace_t trace;
	CTraceFilterWorldOnly filter;

	Vector start = local->GetOrigin(), mins = local->GetCollideable()->OBBMins(), maxs = local->GetCollideable()->OBBMaxs();

	Vector planes;
	auto count = 0;

	for (auto step = 0.f; step <= M_PI * 2.f; step += M_PI / 10.f)
	{
		auto end = start;

		end.x += cos(step) * (1 / 0.0078125f);
		end.y += sin(step) * (1 / 0.0078125f);
		Ray_t ray;
		ray.Init(start, end, mins, maxs);
		ENGINETRACE->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

		if (trace.fraction < 1.f)
		{
			planes += trace.plane.normal;
			count++;
		}
		ticks++;
	}

	planes /= count;

	if (planes.z < 0.1f)
	{
		*plane = planes;
		return true;
	}

	return false;
}

/*
* will_hit_wall
* Calculates if we will hit anything in the future
*/
bool will_hit_wall(float finish_time, float turn_angle, Vector &plane)
{
	C_CSPlayer *local = C_CSPlayer::GetLocalPlayer();
	static auto sv_gravity = CVAR->FindVar("sv_gravity");
	static auto sv_jump_impulse = CVAR->FindVar("sv_jump_impulse");

	bool ground = local->GetFlags() & FL_ONGROUND;
	float gravity_per_tick = sv_gravity->GetFloat() * GLOBALS->interval_per_tick;

	Vector start = local->GetOrigin(), end = start;
	Vector velocity = local->GetVelocity();

	trace_t trace;
	CTraceFilterWorldOnly filter;

	int finish_tick = TIME_TO_TICKS(finish_time);
	double velocity_rotation_angle = RAD2DEG(atan2(velocity.y, velocity.x));
	int ticks_done = 0;

	if (finish_tick <= 0)
		return false;

	while (true)
	{
		float new_angle = velocity_rotation_angle + turn_angle;

		velocity.x = cos(DEG2RAD(new_angle)) * local->GetVelocity().Length2D();
		velocity.y = sin(DEG2RAD(new_angle)) * local->GetVelocity().Length2D();
		velocity.z = ground ? sv_jump_impulse->GetFloat() : velocity.z - gravity_per_tick;

		end += velocity * GLOBALS->interval_per_tick;

		Ray_t ray;
		ray.Init(start, end, local->GetCollideable()->OBBMins(), local->GetCollideable()->OBBMaxs());
		ENGINETRACE->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

		if (trace.fraction != 1.f && trace.plane.normal.z <= 0.7f || trace.startsolid || trace.allsolid) {
			if(!trace.allsolid)
				plane = trace.plane.normal;
			break;
		}

		end = trace.endpos;
		end.z -= 2.f;
		ray.Init(trace.endpos, end, local->GetCollideable()->OBBMins(), local->GetCollideable()->OBBMaxs());
		ENGINETRACE->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);
		ground = (trace.fraction < 1.f || trace.allsolid || trace.startsolid) && trace.plane.normal.z >= 0.7f;

		if (++ticks_done >= finish_tick)
			return false;

		velocity_rotation_angle = new_angle;
	}

	return true;
}

/*
* circle_strafe
* Attempts to automatically strafe around and through objects.
*/
void circle_strafe(float* circle_yaw)
{
	if (!MiscVars::get().g_bCircleStrafe || !MiscVars::get().g_bRageStrafe ||!INPUTSYSTEM->IsButtonDown(MenuVars::get().g_kCircleStrafeKey)) {
		*circle_yaw = g::cmd->viewangles.y;
		return;
	}

	C_CSPlayer *local = C_CSPlayer::GetLocalPlayer();
	const auto min_angle = 2.25f;
	const auto max_angle = 5.f;
	float speed = local->GetVelocity().Length2D();

	const float perfect_strafe = Math::Clamp2(get_move_angle(speed), min_angle, max_angle);
	const float finish_time = Math::Clamp2(320.f / speed, 0.35f, 1.f);

	auto turn_angle = perfect_strafe;

	while (true)
	{
		Vector plane;
		if (!will_hit_wall(finish_time, turn_angle, plane) || turn_angle > max_angle)
			break;

		turn_angle += 0.2f;
	}

	if (turn_angle > max_angle)
	{
		turn_angle = perfect_strafe - 0.2f;
	
		while (true)
		{
			Vector plane;
			if (!will_hit_wall(finish_time, turn_angle, plane) || turn_angle < -min_angle)
				break;
	
			turn_angle -= 0.2f;
		}
	
		if (turn_angle < min_angle)
		{
			Vector plane;
			int ticks = 0;
			if (get_closest_plane(&plane, ticks))
				turn_angle = Math::AngleNormalize(*circle_yaw - RAD2DEG(atan2(plane.y, plane.x)) + 90) * 0.2;
		}
		else
			turn_angle -= 0.2f;
	}
	else
		turn_angle += 0.2f;

	g::cmd->viewangles.y = *circle_yaw = Math::AngleNormalize(*circle_yaw + turn_angle);
	g::cmd->sidemove = copysign(450.f, -turn_angle);
}

void BunnyHop::CreateMove() const
{
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	AutoHop(pLocal);
	AutoStrafe(pLocal);
	static float circle_yaw = 0.f;
	circle_strafe(&circle_yaw);

	Math::Clamp(g::cmd->viewangles);
	Math::ClampMovement(g::cmd);
}

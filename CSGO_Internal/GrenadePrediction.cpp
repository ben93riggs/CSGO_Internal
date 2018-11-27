#include "GrenadePrediction.h"
#include "Globals.h"

#include "CRender.h"

class CGrenadePredictTraceFilter : public ITraceFilter
{
public:
	bool ShouldHitEntity(IHandleEntity* pEntityHandle, int contentsMask)
	{
		if (!pEntityHandle)
			return true;

		ClientClass* ent_client_class = ((IClientEntity*)pEntityHandle)->GetClientClass();

		if (!ent_client_class)
			return false;

		if (ent_client_class->m_ClassID == CCSPlayer)
			return false;

		if (ent_client_class->m_ClassID == CBaseCSGrenade)
			return false;

		return true;// ITraceFilter::ShouldHitEntity((IHandleEntity*)pEntityHandle, contentsMask);
	}
	virtual TraceType GetTraceType() const
	{
		return TraceType::TRACE_EVERYTHING;
	}
	IHandleEntity* pSkip;
	int m_icollisionGroup;
};

bool CCSGrenadeHint::IsGrenade(EItemDefinitionIndex id)
{
	return id >= weapon_flashbang && id <= weapon_incgrenade;
}

//call in createmove
void CCSGrenadeHint::Tick(int buttons)
{
	if (!VisualVars::get().ESP.g_bGrenadePrediction)
		return;

	bool in_attack = buttons & IN_ATTACK;
	bool in_attack2 = buttons & IN_ATTACK2;

	act = (in_attack && in_attack2) ? ACT_LOB :
		(in_attack2) ? ACT_DROP :
		(in_attack) ? ACT_THROW :
		ACT_NONE;
}

//call in overrideview
void CCSGrenadeHint::View()
{
	if (!VisualVars::get().ESP.g_bGrenadePrediction)
		return;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	CBaseCombatWeapon* pWeapon = pLocal->GetActiveWeapon();

	if (!pLocal || !pWeapon)
	{
		type = weapon_none;
		act = ACT_NONE;
		color = Color::White();
		return;
	}

	EItemDefinitionIndex wid = pWeapon ? (EItemDefinitionIndex)pWeapon->GetId() : weapon_none;

	if (IsGrenade(wid) && act != ACT_NONE)
	{
		QAngle angles;
		ENGINE->GetViewAngles(angles);

		type = wid;
		Simulate(pLocal, pWeapon, angles);

		switch (type)
		{
		case weapon_smokegrenade:
			color = Color(0, 255, 255, 255); //cyan
			break;
		case weapon_decoy:
			color = Color(155, 155, 155, 255); //grey
			break;
		case weapon_molotov:
		case weapon_incgrenade:
			color = Color(255, 0, 0, 255); //red
			break;
		case weapon_flashbang:
			color = Color(255, 255, 0, 255); //yellow
			break;
		case weapon_hegrenade:
			color = Color(0, 255, 0, 255); //green
			break;

		default:
			color = Color::White();
			break;
		}
	}
	else
	{
		type = weapon_none;
		color = Color::White();
	}
}

//call in painttraverse to draw
void CCSGrenadeHint::Paint(bool debug)
{
	if (mutex_locked)
		return;

	while (mutex_locked)
		Sleep(0);

	if ((debug || type) && path.size() > 1)
	{
		Vector ab, cd;
		mutex_locked = true;
		Vector prev = path[0];
		for (auto it : path)
		{
			if (Math::WorldToScreen(prev, ab) && Math::WorldToScreen(it, cd))
			{
				CRender::get().DrawLineWithOutline(ab.x, ab.y, cd.x, cd.y, color);

				trace_t tr;
				CGrenadePredictTraceFilter filter;
				filter.m_icollisionGroup = CBaseCSGrenadeProjectile;

				Vector newpos = it + ((it - prev) * .5f);

				Utils::TraceHull(prev, newpos, grenademins, grenademaxs, &filter, &tr);

				if (tr.fraction != 1.0f)
				{
					CRender::get().DrawRect(cd.x - 2, cd.y - 2, 4, 4, Color::Red());
				}
			}

			prev = it;
		}
		mutex_locked = false;

		if (Math::WorldToScreen(prev, cd))
		{
			//CRender::get().DrawTriangleFilled(Vector2D(cd.x - 4, cd.y - 4), Vector2D(cd.x, cd.y + 4), Vector2D(cd.x + 4, cd.y - 4), Color::Red());
			CRender::get().DrawFilledRect(cd.x - 4, cd.y - 4, 8, 8, Color::Red());
		}
	}
}

void CCSGrenadeHint::Setup(C_CSPlayer* pl, CBaseCombatWeapon* weapon, Vector& vecSrc, Vector& vecThrow, const QAngle& angEyeAngles)
{
	QAngle angThrow = angEyeAngles;
	float pitch = angThrow.x;

	if (pitch <= 90.0f)
	{
		if (pitch<-90.0f)
		{
			pitch += 360.0f;
		}
	}
	else
	{
		pitch -= 360.0f;
	}
	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	angThrow.x = a;

	// Gets ThrowVelocity from weapon files
	// Clamped to [15,750]
	float flVel = 750.0f * 0.9f;

	// Do magic on member of grenade object [esi+9E4h]
	// m1=1  m1+m2=0.5  m2=0
	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	// Clamped to [0,1]
	b = b * 0.7f;
	b = b + 0.3f;
	flVel *= b;

	Vector vForward;
	Math::AngleVectors(angThrow, &vForward);

	vecSrc = pl->GetEyePos();
	float off = (power[act] * 12.0f) - 12.0f;
	vecSrc.z += off;

	// Game calls UTIL_TraceHull here with hull and assigns vecSrc tr.endpos
	trace_t tr;
	Vector vecDest = vecSrc;
	vecDest.MultAdd(vForward, 22.0f);
	Utils::TraceHull(vecSrc, vecDest, grenademins, grenademaxs, nullptr, &tr);

	// After the hull trace it moves 6 units back along vForward
	// vecSrc = tr.endpos - vForward * 6
	Vector vecBack = vForward; vecBack *= 6.0f;
	vecSrc = tr.endpos;
	vecSrc -= vecBack;

	// Finally calculate velocity
	vecThrow = pl->GetVelocity();
	vecThrow *= 1.25f;
	vecThrow.MultAdd(vForward, flVel);
}

//////////////////////////////

void CCSGrenadeHint::Simulate(C_CSPlayer* pLocal, CBaseCombatWeapon* weapon, Vector& cam_angles)
{
	Vector vecSrc, vecThrow;
	Setup(pLocal, weapon, vecSrc, vecThrow, cam_angles);

	float interval = GLOBALS->interval_per_tick;

	// Log positions 20 times per sec
	int logstep = static_cast<int>(0.05f / interval);
	int logtimer = 0;

	while (mutex_locked)
		Sleep(0);

	mutex_locked = true;
	path.clear();
	for (unsigned int i = 0; i < path.max_size() - 1; ++i)
	{
		if (!logtimer)//
			path.push_back(vecSrc);//

		int s = Step(vecSrc, vecThrow, i, interval);//

		if (s & 1)
			break;

		// Reset the log timer every logstep OR we bounced
		if (s & 2 || logtimer >= logstep)
			logtimer = 0;
		else
			++logtimer;
	}
	path.push_back(vecSrc);
	mutex_locked = false;
}

int CCSGrenadeHint::Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval)
{
	// Apply gravity
	Vector move;
	AddGravityMove(move, vecThrow, interval, false);

	// Push entity
	trace_t tr;
	PushEntity(vecSrc, move, tr);

	int result = 0;
	// Check ending conditions
	if (CheckDetonate(vecThrow, tr, tick, interval))
	{
		result |= 1;
	}

	// Resolve collisions
	if (tr.fraction != 1.0f)
	{
		result |= 2; // Collision!
		ResolveFlyCollisionCustom(tr, vecThrow, interval);
	}

	// Set new position
	vecSrc = tr.endpos;

	return result;
}

bool CCSGrenadeHint::CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval) const
{
	switch (type)
	{
	case weapon_smokegrenade:
	case weapon_decoy:
		// Velocity must be <0.1, this is only checked every 0.2s
		if (vecThrow.Length2D() < 0.1f)
		{
			int det_tick_mod = static_cast<int>(0.2f / interval);
			return !(tick % det_tick_mod);
		}
		return false;

	case weapon_molotov:
	case weapon_incgrenade:
		// Detonate when hitting the floor
		if (tr.fraction != 1.0f && tr.plane.normal.z>0.7f)
			return true;
		// OR we've been flying for too long

	case weapon_flashbang:
	case weapon_hegrenade:
		// Pure timer based, detonate at 1.5s, checked every 0.2s
		return static_cast<float>(tick) * interval > 1.5f && !(tick % static_cast<int>(0.2f / interval));

	default:
		assert(false);
		return false;
	}
}

void CCSGrenadeHint::TraceHull(Vector& src, Vector& end, trace_t& tr)
{
	/*Setup grenade hull
	static const Vector hull[2] = { Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f) };*/

	// FIXME! Bad codings...
	CGrenadePredictTraceFilter filter;
	filter.m_icollisionGroup = CBaseCSGrenadeProjectile;

	Utils::TraceHull(src, end, grenademins, grenademaxs, &filter, &tr);
}

void CCSGrenadeHint::AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground)
{
	Vector basevel = Vector(0.0f, 0.0f, 0.0f);

	move.x = (vel.x + basevel.x) * frametime;
	move.y = (vel.y + basevel.y) * frametime;

	if (onground)
	{
		move.z = (vel.z + basevel.z) * frametime;
	}
	else
	{
		// Game calls GetActualGravity( this );
		float gravity = 800.0f * 0.4f;

		float newZ = vel.z - (gravity * frametime);
		move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;

		vel.z = newZ;
	}
}

void CCSGrenadeHint::PushEntity(Vector& src, const Vector& move, trace_t& tr)
{
	Vector vecAbsEnd = src;
	vecAbsEnd += move;

	// Trace through world
	TraceHull(src, vecAbsEnd, tr);
}

void CCSGrenadeHint::ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval)
{
	// Calculate elasticity
	float flSurfaceElasticity = 1.0;  // Assume all surfaces have the same elasticity
	float flGrenadeElasticity = 0.45f; // GetGrenadeElasticity()
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity > 0.9f) flTotalElasticity = 0.9f;
	if (flTotalElasticity < 0.0f) flTotalElasticity = 0.0f;

	// Calculate bounce
	Vector vecAbsVelocity;
	PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;

	// Stop completely once we move too slow
	float flSpeedSqr = vecAbsVelocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
	if (flSpeedSqr < flMinSpeedSqr)
	{
		vecAbsVelocity.Zero();
	}

	// Stop if on ground
	if (tr.plane.normal.z > 0.7f)
	{
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity *= (1.0f - tr.fraction) * interval;
		PushEntity(tr.endpos, vecAbsVelocity, tr);
	}
	else
	{
		vecVelocity = vecAbsVelocity;
	}
}

int CCSGrenadeHint::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	int blocked = 0;

	float angle = normal[2];

	if (angle > 0)
	{
		blocked |= 1;        // floor
	}
	if (!angle)
	{
		blocked |= 2;        // step
	}

	float backoff = in.Dot(normal) * overbounce;

	for (int i = 0; i < 3; i++)
	{
		float change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		{
			out[i] = 0;
		}
	}

	return blocked;
}
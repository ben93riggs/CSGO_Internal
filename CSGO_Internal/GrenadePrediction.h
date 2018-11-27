#pragma once
#include "Singleton.h"
#include "SourceEngine/SDK.hpp"

class C_CSPlayer;
class CBaseCombatWeapon;

enum acts
{
	ACT_NONE = 0,
	ACT_THROW,
	ACT_DROP,
	ACT_LOB
};

class CCSGrenadeHint : public singleton<CCSGrenadeHint>
{
public:
	int GrenadeType = 0;
	acts act = ACT_NONE;
	bool mutex_locked = false;
	std::vector<Vector> path;
	Vector grenademins = Vector(-2.0f, -2.0f, -2.0f);
	Vector grenademaxs = Vector(2.0f, 2.0f, 2.0f);
	EItemDefinitionIndex type = weapon_none;
	Color color = Color::White();


	static bool IsGrenade(EItemDefinitionIndex id);
	void Tick(int buttons);
	void View();
	void Paint(bool debug);
	void Setup(C_CSPlayer* pl, CBaseCombatWeapon* weapon, Vector& vecSrc, Vector& vecThrow, const QAngle& angEyeAngles);
	void Simulate(C_CSPlayer * pLocal, CBaseCombatWeapon * weapon, Vector & cam_angles);

	int Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval);
	bool CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval) const;
	void TraceHull(Vector& src, Vector& end, trace_t& tr);
	static void AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground);
	void PushEntity(Vector& src, const Vector& move, trace_t& tr);
	void ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval);
	static int PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);
};

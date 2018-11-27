#pragma once
#include "Singleton.h"

#include "SourceEngine/SDK.hpp"
#include "C_CSPlayer.h"


class CBaseCombatWeapon;

class CTraceFilterNoPlayers : public CTraceFilter
{
public:
	CTraceFilterNoPlayers() {}
	virtual bool ShouldHitEntity(IClientEntity *pServerEntity, int contentsMask)
	{
		IClientUnknown *pUnk = (IClientUnknown*)pServerEntity;

		if (!pUnk)
			return false;

		IClientEntity *pEnt = pUnk->GetIClientEntity();

		if (!pEnt)
			return false;

		ClientClass* ent_client_class = pEnt->GetClientClass();

		if (!ent_client_class)
			return false;

		return ent_client_class->m_ClassID != CCSPlayer;
	}
};

class CAutoWall : public singleton<CAutoWall>
{
public:
	static bool DidHitNonWorldEntity(IClientEntity* m_pEnt);
	static void TraceLine(Vector& absStart, Vector& absEnd, unsigned mask, IClientEntity* ignore, CGameTrace* ptr);
	static void ClipTraceToPlayers(Vector& absStart, Vector absEnd, unsigned mask, ITraceFilter* filter, CGameTrace* tr);
	static void GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type);
	void scale_damage(CGameTrace& enterTrace, const CCSWeaponInfo* weaponData, float& currentDamage) const;
	static bool TraceToExit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction);
	bool HandleBulletPenetration(C_CSPlayer* pLocal, const CCSWeaponInfo* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration) const;
	bool FireBullet(C_CSPlayer* pLocal, CBaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage, int& possibleHitsRemaining) const;
	float can_hit(Vector& point, C_CSPlayer* pLocal) const;
};

typedef int(__stdcall* UTIL_TraceLine_t)(const Vector&, const Vector&, unsigned int, const IClientEntity*, int, trace_t*);
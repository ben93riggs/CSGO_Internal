#include "Autowall.h"
#include "Globals.h"
#include <intrin.h>

bool CAutoWall::DidHitNonWorldEntity(IClientEntity* m_pEnt) 
{
	return m_pEnt != nullptr && m_pEnt != ENTITYLIST->GetClientEntity(0);
}

void CAutoWall::TraceLine(Vector& absStart, Vector& absEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr)
{
	Ray_t ray;
	ray.Init(absStart, absEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	ENGINETRACE->TraceRay(ray, mask, &filter, ptr);
}

void CAutoWall::ClipTraceToPlayers(Vector& absStart, Vector absEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr)
{
	static uintptr_t clptrtp = Utils::FindSignature(Utils::get_client_dll(), XorStr("53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B 04 89 6C 24 ? 8B EC 81 EC ? ? ? ? 8B 43 10 56 57 52 F3 0F 10 40"));
			
	if (!clptrtp)
		return;

	__asm
	{
		mov eax, filter
		lea ecx, tr
		push ecx
		push eax
		push mask
		lea edx, absEnd
		lea ecx, absStart
		call clptrtp
		add esp, 0xC
	}
}

//Legacy Function
void CAutoWall::GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type)
{
	if (sv_penetration_type)
	{
		maxRange = 35.0;
		maxDistance = 3000.0;
	}
	else
	{
		//Play tribune to framerate. Thanks, stringcompare
		//Regardless I doubt anyone will use the old penetration system anyway; so it won't matter much.
		if (!strcmp(bulletType, XorStr("BULLET_PLAYER_338MAG")))
		{
			maxRange = 45.0;
			maxDistance = 8000.0;
		}
		if (!strcmp(bulletType, XorStr("BULLET_PLAYER_762MM")))
		{
			maxRange = 39.0;
			maxDistance = 5000.0;
		}
		if (!strcmp(bulletType, XorStr("BULLET_PLAYER_556MM")) || !strcmp(bulletType, XorStr("BULLET_PLAYER_556MM_SMALL")) || !strcmp(bulletType, XorStr("BULLET_PLAYER_556MM_BOX")))
		{
			maxRange = 35.0;
			maxDistance = 4000.0;
		}
		if (!strcmp(bulletType, XorStr("BULLET_PLAYER_57MM")))
		{
			maxRange = 30.0;
			maxDistance = 2000.0;
		}
		if (!strcmp(bulletType, XorStr("BULLET_PLAYER_50AE")))
		{
			maxRange = 30.0;
			maxDistance = 1000.0;
		}
		if (!strcmp(bulletType, XorStr("BULLET_PLAYER_357SIG")) || !strcmp(bulletType, XorStr("BULLET_PLAYER_357SIG_SMALL")) || !strcmp(bulletType, XorStr("BULLET_PLAYER_357SIG_P250")) || !strcmp(bulletType, XorStr("BULLET_PLAYER_357SIG_MIN")))
		{
			maxRange = 25.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, XorStr("BULLET_PLAYER_9MM")))
		{
			maxRange = 21.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, XorStr("BULLET_PLAYER_45ACP")))
		{
			maxRange = 15.0;
			maxDistance = 500.0;
		}
		if (!strcmp(bulletType, XorStr("BULLET_PLAYER_BUCKSHOT")))
		{
			maxRange = 0.0;
			maxDistance = 0.0;
		}
	}
}

void CAutoWall::scale_damage(CGameTrace& enterTrace, const CCSWeaponInfo* weaponData, float& currentDamage) const
{
	//Cred. to N0xius for reversing this.
	//TODO: _xAE^; look into reversing this yourself sometime

	bool hasHeavyArmor = false;//((C_CSPlayer*)enterTrace.m_pEnt)->HeavyArmor();
	int armorValue = ((C_CSPlayer*)enterTrace.m_pEnt)->GetArmorValue();

	//screw making a new function, lambda beste. ~ Does the person have armor on for the hitbox checked?
	auto IsArmored = [&enterTrace]()->bool
	{
		C_CSPlayer* targetEntity = (C_CSPlayer*)enterTrace.m_pEnt;
		switch (enterTrace.hitgroup)
		{
		case HITGROUP_HEAD:
			return !!(C_CSPlayer*)targetEntity->HasHelmet(); //screw compiler errors - force-convert it to a bool via (!!)
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	switch (enterTrace.hitgroup)
	{
	case HITGROUP_HEAD:
		currentDamage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
		break;
	case HITGROUP_STOMACH:
		currentDamage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		currentDamage *= 0.75f;
		break;
	default:
		break;
	}

	if (armorValue > 0 && IsArmored())
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weaponData->flArmorRatio / 2.f;

		//Damage gets modified for heavy armor users
		if (hasHeavyArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if (hasHeavyArmor)
			NewDamage *= 0.85f;

		if (((currentDamage - (currentDamage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
			NewDamage = currentDamage - (armorValue / armorBonusRatio);

		currentDamage = NewDamage;
	}
}

bool CAutoWall::TraceToExit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction)
{
	/*
	Masks used:
	MASK_SHOT_HULL					 = 0x0600400B
	CONTENTS_HITBOX					 = 0x40000000
	MASK_SHOT_HULL | CONTENTS_HITBOX = 0x4600400B
	*/

	float maxDistance = 90.f, rayExtension = 4.f, currentDistance = 0;
	int firstContents = 0;

	while (currentDistance <= maxDistance)
	{
		//Add extra distance to our ray
		currentDistance += rayExtension;

		//Multiply the direction vector to the distance so we go outwards, add our position to it.
		Vector start = startPosition + direction * currentDistance;

		if (!firstContents)
			firstContents = ENGINETRACE->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr); /*0x4600400B*/
		int pointContents = ENGINETRACE->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (!(pointContents & MASK_SHOT_HULL) || pointContents & CONTENTS_HITBOX && pointContents != firstContents) /*0x600400B, *0x40000000*/
		{
			//Let's setup our end position by deducting the direction by the extra added distance
			Vector end = start - (direction * rayExtension);

			//Let's cast a ray from our start pos to the end pos
			TraceLine(start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);

			//Let's check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if (exitTrace.startsolid && exitTrace.surface.flags & SURF_HITBOX)
			{
				TraceLine(start, startPosition, MASK_SHOT_HULL, exitTrace.m_pEnt, &exitTrace);

				if (exitTrace.DidHit() && !exitTrace.startsolid)
				{
					start = exitTrace.endpos;
					return true;
				}

				continue;
			}

			//Can we hit? Is the wall solid?
			if (exitTrace.DidHit() && !exitTrace.startsolid)
			{
				//Is the wall a breakable? If so, let's shoot through it.
				if (((C_CSPlayer*)enterTrace.m_pEnt)->IsBreakableEnt() && (((C_CSPlayer*)exitTrace.m_pEnt)->IsBreakableEnt()))
					return true;

				if (enterTrace.surface.flags & SURF_NODRAW || !(exitTrace.surface.flags & SURF_NODRAW) && (exitTrace.plane.normal.Dot(direction) <= 1.f))
				{
					float multAmount = exitTrace.fraction * 4.f;
					start -= direction * multAmount;
					return true;
				}

				continue;
			}

			if (!exitTrace.DidHit() || exitTrace.startsolid)
			{
				if (DidHitNonWorldEntity(enterTrace.m_pEnt) && ((C_CSPlayer*)enterTrace.m_pEnt)->IsBreakableEnt())
				{
					exitTrace = enterTrace;
					exitTrace.endpos = start + direction;
					return true;
				}

				continue;
			}
		}
	}
	return false;
}

bool CAutoWall::HandleBulletPenetration(C_CSPlayer* pLocal, const CCSWeaponInfo* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration) const
{
	trace_t exitTrace;

	C_CSPlayer* pEnemy = (C_CSPlayer*)enterTrace.m_pEnt;

	if (!pEnemy)
		return false;

	surfacedata_t *enterSurfaceData = PHYSPROPS->GetSurfaceData(enterTrace.surface.surfaceProps);
	int enterMaterial = enterSurfaceData->game.material;
	float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;

	float enterDamageModifier = enterSurfaceData->game.flDamageModifier;
	float thickness, finalDamageModifier, combinedPenetrationModifier;
	bool isSolidSurf = ((enterTrace.contents >> 3) & CONTENTS_SOLID);
	bool isLightSurf = ((enterTrace.surface.flags >> 7) & SURF_LIGHT);

	if (possibleHitsRemaining <= 0
		|| (!possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != CHAR_TEX_GRATE && enterMaterial != CHAR_TEX_GLASS)
		|| weaponData->flPenetration <= 0.f
		|| !TraceToExit(enterTrace, exitTrace, enterTrace.endpos, direction)
		&& !(ENGINETRACE->GetPointContents(enterTrace.endpos, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
		return false;

	surfacedata_t *exitSurfaceData = PHYSPROPS->GetSurfaceData(exitTrace.surface.surfaceProps);

	if (!exitSurfaceData)
		return false;

	int exitMaterial = exitSurfaceData->game.material;
	float exitSurfPenetrationModifier = exitSurfaceData->game.flPenetrationModifier;
	float exitDamageModifier = exitSurfaceData->game.flDamageModifier;

	//Are we using the newer penetration system?
	if (sv_penetration_type)
	{
		if (enterMaterial == CHAR_TEX_GRATE || enterMaterial == CHAR_TEX_GLASS)
		{
			combinedPenetrationModifier = 3.f;
			finalDamageModifier = 0.05f;
		}
		else if (isSolidSurf || isLightSurf)
		{
			combinedPenetrationModifier = 1.f;
			finalDamageModifier = 0.16f;
		}
		else if (enterMaterial == CHAR_TEX_FLESH && (pLocal->GetTeam() == pEnemy->GetTeam() && ff_damage_reduction_bullets == 0.f))
		{
			//Look's like you aren't shooting through your teammate today
			if (ff_damage_bullet_penetration == 0.f)
				return false;

			//Let's shoot through teammates and get kicked for teamdmg! Whatever, atleast we did damage to the enemy. I call that a win.
			combinedPenetrationModifier = ff_damage_bullet_penetration;
			finalDamageModifier = 0.16f;
		}
		else
		{
			combinedPenetrationModifier = (enterSurfPenetrationModifier + exitSurfPenetrationModifier) / 2.f;
			finalDamageModifier = 0.16f;
		}

		//Do our materials line up?
		if (enterMaterial == exitMaterial)
		{
			if (exitMaterial == CHAR_TEX_CARDBOARD || exitMaterial == CHAR_TEX_WOOD)
				combinedPenetrationModifier = 3.f;
			else if (exitMaterial == CHAR_TEX_PLASTIC)
				combinedPenetrationModifier = 2.f;
		}

		//Calculate thickness of the wall by getting the length of the range of the trace and squaring
		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();
		float modifier = fmaxf(1.f / combinedPenetrationModifier, 0.f);

		//This calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
		float lostDamage = fmaxf(
			((modifier * thickness) / 24.f)
			+ ((currentDamage * finalDamageModifier)
				+ (fmaxf(3.75f / penetrationPower, 0.f) * 3.f * modifier)), 0.f);

		//Did we loose too much damage?
		if (lostDamage > currentDamage)
			return false;

		//We can't use any of the damage that we've lost
		if (lostDamage > 0.f)
			currentDamage -= lostDamage;

		//Do we still have enough damage to deal?
		if (currentDamage < 1.f)
			return false;

		eyePosition = exitTrace.endpos;
		--possibleHitsRemaining;

		return true;
	}
	else //Legacy penetration system
	{
		combinedPenetrationModifier = 1.f;

		if (isSolidSurf || isLightSurf)
			finalDamageModifier = 0.99f; //Good meme :^)
		else
		{
			finalDamageModifier = fminf(enterDamageModifier, exitDamageModifier);
			combinedPenetrationModifier = fminf(enterSurfPenetrationModifier, exitSurfPenetrationModifier);
		}

		if (enterMaterial == exitMaterial && (exitMaterial == CHAR_TEX_METAL || exitMaterial == CHAR_TEX_WOOD))
			combinedPenetrationModifier += combinedPenetrationModifier;

		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();

		if (sqrt(thickness) <= combinedPenetrationModifier * penetrationPower)
		{
			currentDamage *= finalDamageModifier;
			eyePosition = exitTrace.endpos;
			--possibleHitsRemaining;

			return true;
		}

		return false;
	}
}

bool CAutoWall::FireBullet(C_CSPlayer* pLocal, CBaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage, int& possibleHitsRemaining) const
{
	if (!pWeapon)
		return false;

	static auto UTIL_ClipTraceToPlayers = [](const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr) 
	{
		static uintptr_t clptrtp = Utils::FindSignature(Utils::get_client_dll(), XorStr("53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B 04 89 6C 24 ? 8B EC 81 EC ? ? ? ? 8B 43 10 56 57 52 F3 0F 10 40"));
			
		if (!clptrtp)
			return false;
			
		__asm {
			push tr
			push filter
			push mask
			lea edx, vecAbsEnd
			lea ecx, vecAbsStart
			call clptrtp
			add esp, 0xC
		}
	};

	//	  Current bullet travel Power to penetrate Distance to penetrate Range               Player bullet reduction convars			  Amount to extend ray by
	float currentDistance = 0.f, penetrationPower, penetrationDistance, rayExtension = 40.f;

	Vector eyePosition = pLocal->GetEyePos();

	//For being superiour when the server owners think your autowall isn't well reversed. Imagine a meme HvH server with the old penetration system- pff
	static ConVar* penetrationSystem = CVAR->FindVar(XorStr("sv_penetration_type"));
	static ConVar* damageReductionBullets = CVAR->FindVar(XorStr("ff_damage_reduction_bullets"));
	static ConVar* damageBulletPenetration = CVAR->FindVar(XorStr("ff_damage_bullet_penetration"));

	if (!penetrationSystem || !damageReductionBullets || !damageBulletPenetration)
		return false;

	bool sv_penetration_type = penetrationSystem->GetBool();
	float ff_damage_reduction_bullets = damageReductionBullets->GetFloat();
	float ff_damage_bullet_penetration = damageBulletPenetration->GetFloat();

	const CCSWeaponInfo* weaponData = pWeapon->GetCSWpnData();
	
	if (!weaponData)
		return false;

	float maxRange = weaponData->flRange;

	GetBulletTypeParameters(penetrationPower, penetrationDistance, weaponData->szBulletType, sv_penetration_type);

	if (sv_penetration_type)
		penetrationPower = weaponData->flPenetration;

	//This gets set in FX_Firebullets to 4 as a pass-through value.
	//CS:GO has a maximum of 4 surfaces a bullet can pass-through before it 100% stops.
	//Excerpt from Valve: https://steamcommunity.com/sharedfiles/filedetails/?id=275573090
	//"The total number of surfaces any bullet can penetrate in a single flight is capped at 4." -CS:GO Official

	//Set our current damage to what our gun's initial damage reports it will do
	currentDamage = weaponData->iDamage;

	trace_t enterTrace;
	CTraceFilter filter;
	filter.pSkip = pLocal;

	//If our damage is greater than (or equal to) 1, and we can shoot, let's shoot.
	while (possibleHitsRemaining > 0 && currentDamage >= 1.f)
	{
		//Calculate max bullet range
		maxRange -= currentDistance;

		//Create endpoint of bullet
		Vector end = eyePosition + direction * maxRange;

		Utils::TraceLine(eyePosition, end, MASK_SHOT_HULL | CONTENTS_HITBOX, pLocal, &enterTrace);
		UTIL_ClipTraceToPlayers(eyePosition, end + direction * rayExtension, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enterTrace);

		//We have to do this *after* tracing to the player.
		surfacedata_t* enterSurfaceData = PHYSPROPS->GetSurfaceData(enterTrace.surface.surfaceProps);

		if (!enterSurfaceData)
			return false;

		float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
		int enterMaterial = enterSurfaceData->game.material;

		//"Fraction == 1" means that we didn't hit anything. We don't want that- so let's break on it.
		if (enterTrace.fraction == 1.f)
			break;

		//calculate the damage based on the distance the bullet traveled.
		currentDistance += enterTrace.fraction * maxRange;

		//Let's make our damage drops off the further away the bullet is.
		currentDamage *= pow(weaponData->flRangeModifier, (currentDistance / 500.f));

		//Sanity checking / Can we actually shoot through?
		if (currentDistance > penetrationDistance && weaponData->flPenetration > 0.f || enterSurfPenetrationModifier < 0.1f)
			break;

		//This looks dumb if we put it into 1 long line of code.
		bool canDoDamage = (enterTrace.hitgroup != HITGROUP_GEAR && enterTrace.hitgroup != HITGROUP_GENERIC);
		bool isPlayer = ((enterTrace.m_pEnt)->GetClientClass()->m_ClassID == CCSPlayer);
		bool isEnemy = (((C_CSPlayer*)pLocal)->GetTeam() != ((C_CSPlayer*)enterTrace.m_pEnt)->GetTeam());
		bool onTeam = (((C_CSPlayer*)enterTrace.m_pEnt)->GetTeam() == 3 || ((C_CSPlayer*)enterTrace.m_pEnt)->GetTeam() == 2);

		//TODO: Team check config
		if (canDoDamage && isPlayer && isEnemy && onTeam)
		{
			scale_damage(enterTrace, weaponData, currentDamage);
			return true;
		}

		//Calling HandleBulletPenetration here reduces our penetrationCounter, and if it returns true, we can't shoot through it.
		if (!HandleBulletPenetration(pLocal, weaponData, enterTrace, eyePosition, direction, possibleHitsRemaining, currentDamage, penetrationPower, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
			break;
	}
	return false;
}

float CAutoWall::can_hit(Vector &point, C_CSPlayer* pLocal) const
{
	if (!pLocal)
		return 0;

	Vector angles, direction;
	Vector tmp = point - pLocal->GetEyePos();
	
	Math::VectorAngles(tmp, angles);
	Math::AngleVectors(angles, &direction);
	direction.NormalizeInPlace();

	float currentDamage = 0.f;
	int possibleHitsRemaining = 4;
	if (FireBullet(pLocal, pLocal->GetActiveWeapon(), direction, currentDamage, possibleHitsRemaining))
	{
		if (possibleHitsRemaining < 4 && !WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAutoWall)
			return 0.f;

		return currentDamage;
	}

	return 0.f; //That wall is just a bit too thick buddy
}

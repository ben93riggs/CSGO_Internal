#pragma once
#include "SourceEngine/IClientEntity.hpp"
#include "NetvarManager.hpp"
#include "XorStr.hpp"
#include <deque>
#include "CBoneAccessor.h"

#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / GLOBALS->interval_per_tick))
#define TICKS_TO_TIME(t) (GLOBALS->interval_per_tick * (t))

struct C_CSGOPlayerAnimState;
class CCSGOPlayerAnimState;

inline unsigned int FindInDataMap(datamap_t *pMap, const char *name)
{
	while (pMap)
	{
		for (int i = 0; i<pMap->dataNumFields; i++)
		{
			if (pMap->dataDesc[i].fieldName == NULL)
				continue;

			if (strcmp(name, pMap->dataDesc[i].fieldName) == 0)
				return pMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

			if (pMap->dataDesc[i].fieldType == FIELD_EMBEDDED)
			{
				if (pMap->dataDesc[i].td)
				{
					unsigned int offset;

					if ((offset = FindInDataMap(pMap->dataDesc[i].td, name)) != 0)
						return offset;
				}
			}
		}
		pMap = pMap->baseMap;
	}

	return 0;
}

class CBaseEntity : public IClientEntity
{
public:
	NETVAR(unsigned, GetModelIndex, XorStr("DT_BaseEntity"), XorStr("m_nModelIndex"))
	NETVAR_OFFSET(int, GetIndex, 0x4, XorStr("DT_BaseEntity"), XorStr("m_bIsAutoaimTarget"))
	void SetModelIndex(int index)
	{
		CallVFunction<void(__thiscall*)(CBaseEntity*, int)>(this, 75)(this, index);
	}
	void SetWeaponModel(const char* Filename, IClientEntity* Weapon)
	{
		return CallVFunction<void(__thiscall*)(void*, const char*, IClientEntity*)>(this, 242)(this, Filename, Weapon);
	}
};

class C_CSPlayer : public CBaseEntity
{
	template<class T>
	T GetFieldValue(int offset)
	{
		return *(T*)((DWORD)this + offset);
	}

	datamap_t *GetDataDescMap()
	{
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return CallVFunction<o_GetPredDescMap>(this, 15)(this);
	}

	datamap_t *GetPredDescMap()
	{
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return CallVFunction<o_GetPredDescMap>(this, 17)(this);
	}

public:
	static C_CSPlayer* GetLocalPlayer();
	static C_CSPlayer* GetPlayerByIndex(int i_target_idx);

	Vector &GetAbsOrigin();
	QAngle &GetAbsAngles();
	void SetAbsAngles( const QAngle angles );
	void SetAbsOrigin(const Vector& origin);

	int GetChokedPackets();
	int GetNumberOfTicksChoked() const;
	
	NETVAR(bool, m_bClientSideAnimation, XorStr("DT_BaseAnimating"), XorStr("m_bClientSideAnimation"))
	NETVAR(bool, GetSpotted, XorStr("DT_BaseEntity"), XorStr("m_bSpotted"))
	NETVAR(bool, HasHelmet, XorStr("DT_CSPlayer"), XorStr("m_bHasHelmet"))
	NETVAR(bool, GetGunGameImmunity, XorStr("DT_CSPlayer"), XorStr("m_bGunGameImmunity"))
	NETVAR(bool, IsBroken, XorStr("DT_BreakableSurface"), XorStr("m_bIsBroken"))
	NETVAR(bool, IsScoped, XorStr("DT_CSPlayer"), XorStr("m_bIsScoped"))
	NETVAR(bool, m_lifeState, XorStr("DT_BasePlayer"), XorStr("m_lifeState")) //dont access this directly, use IsAlive()
	NETVAR(CBaseHandle, GetViewModel, XorStr("DT_BasePlayer"), XorStr("m_hViewModel[0]"))
	NETVAR(CBaseHandle, GetObserverTargetHandle, XorStr("DT_BasePlayer"), XorStr("m_hObserverTarget"))
	NETVAR(CBaseHandle, GetMyWeapons, XorStr("DT_BaseCombatCharacter"), XorStr("m_hMyWeapons"))
	PNETVAR(char*, GetLastPlace, XorStr("DT_BasePlayer"), XorStr("m_szLastPlaceName"))
	NETVAR_STATIC(C_CSGOPlayerAnimState*, GetPlayerAnimState, 0x3884) //0x3894?

	using weaponarray = std::array<CBaseHandle, 48>;
	NETVAR(weaponarray, GetWeapons, XorStr("DT_BaseCombatCharacter"), XorStr("m_hMyWeapons"))
	PNETVAR(CBaseHandle, GetWearables, XorStr("DT_BaseCombatCharacter"), XorStr("m_hMyWearables"))
	NETVAR(float, GetFlashDuration, XorStr("DT_CSPlayer"), XorStr("m_flFlashDuration"))
	NETVAR(float, GetNextAttack, XorStr("DT_BaseCombatCharacter"), XorStr("bcc_localdata"), XorStr("m_flNextAttack"))
	NETVAR(float, GetFallVelocity, XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_flFallVelocity"))
	NETVAR(float, GetMaxSpeed, XorStr("DT_BasePlayer"), XorStr("m_flMaxspeed"))
	NETVAR(float, GetAnimTime, XorStr("DT_BaseEntity"), XorStr("m_flAnimTime"))
	NETVAR(float, GetSimulationTime, XorStr("DT_BaseEntity"), XorStr("m_flSimulationTime"))
	NETVAR_OFFSET(float, m_flOldSimulationTime, 0x4, XorStr("DT_BaseEntity"), XorStr("m_flSimulationTime"))
	NETVAR(float, GetLowerBodyYawTarget, XorStr("DT_CSPlayer"), XorStr("m_flLowerBodyYawTarget"))
	NETVAR(float, GetDuckAmount, XorStr("DT_BasePlayer"), XorStr("m_flDuckAmount"))
	NETVAR(float, GetDuckSpeed, XorStr("DT_BasePlayer"), XorStr("m_flDuckSpeed"))
	NETVAR(float, GetFriction, XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_flFriction"))
	NETVAR(float, GetStepSize, XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_flStepSize"))
	NETVAR_STATIC(float, GetSpawnTime, 0xA290)
	NETVAR(int, GetObserverMode, XorStr("DT_BasePlayer"), XorStr("m_iObserverMode"))
	NETVAR(int, GetHealth, XorStr("DT_BasePlayer"), XorStr("m_iHealth"))
	NETVAR(int, GetArmorValue, XorStr("DT_CSPlayer"), XorStr("m_ArmorValue"))
	NETVAR(int, GetAmmo, XorStr("DT_BasePlayer"), XorStr("m_iAmmo"))
	NETVAR(int, GetTeam, XorStr("DT_BaseEntity"), XorStr("m_iTeamNum"))
	NETVAR(int, GetFlags, XorStr("DT_BasePlayer"), XorStr("m_fFlags"))
	NETVAR(int, GetShotsFired, XorStr("DT_CSPlayer"), XorStr("cslocaldata"), XorStr("m_iShotsFired"))
	NETVAR(int, GetPlayerHitboxSet, XorStr("DT_BaseAnimating"), XorStr("m_nHitboxSet"))
	NETVAR(int, GetCollisionGroup, XorStr("DT_BaseEntity"), XorStr("m_CollisionGroup"))
	NETVAR_STATIC(int, GetMoveType, 0x258)
	NETVAR_STATIC(int, GetNumAnimOverlays, 0x297C)
	PNETVAR_STATIC(int, m_takedamage, 0x27C)
	NETVAR(int, GetTickBase, XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_nTickBase"))
	NETVAR_OFFSET(matrix3x4_t, GetCoordinateFrame, -0x30, XorStr("DT_BaseEntity"), XorStr("m_CollisionGroup"))
	NETVAR(QAngle, ViewPunch, XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_viewPunchAngle"))
	NETVAR(QAngle, AimPunch, XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_aimPunchAngle"))
	NETVAR(QAngle, GetEyeAngles, XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles[0]"))
	NETVAR(QAngle, GetRotation, XorStr("DT_BaseEntity"), XorStr("m_angRotation")) //angle.x, angle.y, angle.z
	NETVAR_OFFSET(QAngle, GetAbsRotation, -0xC, XorStr("DT_BaseEntity"), XorStr("m_angRotation")) //0, 0, 0
	NETVAR_STATIC(QAngle, GetAngAbsRotation, 0xC4) //0, angle.y, 0
	NETVAR(ULONG, GetOwnerEntity, XorStr("DT_BaseEntity"), XorStr("m_hOwnerEntity"))
	NETVAR(Vector, GetOrigin, XorStr("DT_BaseEntity"), XorStr("m_vecOrigin"))
	NETVAR_STATIC(Vector, GetOrigin2, 0xAC) //absorigin
	NETVAR_STATIC(Vector, GetOldOrigin, 0x3A8)
	NETVAR(Vector, GetViewOffset, XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_vecViewOffset[0]"))
	NETVAR(Vector, GetVelocity, XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_vecVelocity[0]"))
	NETVAR(Vector, GetMins, XorStr("DT_BaseEntity"), XorStr("m_Collision"), XorStr("m_vecMins"))
	NETVAR(Vector, GetMaxs, XorStr("DT_BaseEntity"), XorStr("m_Collision"), XorStr("m_vecMaxs"))
	NETVAR_OFFSET(Vector, vecAngles, 0x4, XorStr("DT_BasePlayer"), XorStr("pl"), XorStr("deadflag"))
	NETVAR(int, GetSequence, XorStr("DT_BaseAnimating"), XorStr("m_nSequence"))
	NETVAR(float, GetCycle, XorStr("DT_BaseAnimating"), XorStr("serveranimdata"), XorStr("m_flCycle"))

	//credits xxharcs
	float_t& m_surfaceFriction();
	Vector& m_vecBaseVelocity();
	float_t& m_flMaxspeed();
	VarMapping_t* C_CSPlayer::VarMapping();

	void InvalidatePhysicsRecursive(int nChangeFlags);
	CBoneAccessor* GetBoneAccessor();
	void InvalidateBoneCache();
	bool SetupBones(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);
	void UpdateIKLocks(float currentTime);
	void CalculateIKLocks(float currentTime);
	void StandardBlendingRules(studiohdr_t* pStudioHdr, Vector* pos, Quaternion* q, float currentTime, int boneMask);
	void ControlMouth(studiohdr_t* pStudioHdr);
	void BuildTransformations(studiohdr_t* pStudioHdr, Vector* pos, Quaternion* q, const matrix3x4_t& cameraTransform, int boneMask, byte* computed);
	bool GetHitboxPosition(int hitbox, matrix3x4_t* matrix, Vector& out);

	bool IsPlayer();
	CBaseCombatWeapon* GetActiveWeapon();
	std::string sGetName() const;
	bool IsInReload();
	bool bCanFire();
	static bool IsVisible(Vector &bonepos);
	bool HandleBoneSetup(int32_t boneMask, matrix3x4_t* boneOut, float_t curtime);
	bool IsAlive() const;
	Vector GetEyePos() const;
	std::array<float, 24>& GetPoseParameters();
	void SetPoseAngles(float_t yaw, float_t pitch);
	void SetPoseParameters(std::array<float, 24>&);
	float GetBombTimer();
	void ExtrapolateTick(Vector& p0) const;
	void ExtrapolateChokedTicks();
	UINT PhysicsSolidMaskForEntity();
	void UpdateClientSideAnimation();
	C_AnimationLayer* GetAnimOverlays();
	C_AnimationLayer& C_CSPlayer::GetAnimOverlay2(int Index);
	std::array<C_AnimationLayer, 13> C_CSPlayer::GetAnimOverlay();
	C_AnimationLayer* GetAnimOverlay(int i);
	int GetSequenceActivity(int sequence);
	bool IsBreakableEnt();
};

class CBaseCombatWeapon : public CBaseEntity {
public:
	NETVAR(bool, IsPinPulled, XorStr("DT_BaseCSGrenade"), XorStr("m_bPinPulled"))
	NETVAR(bool, ReloadComplete, XorStr("DT_WeaponCSBase"), XorStr("m_bReloadVisuallyComplete"))
	NETVAR_OFFSET(bool, IsInReload, 0x71, XorStr("DT_BaseCombatWeapon"), XorStr("LocalActiveWeaponData"), XorStr("m_flNextPrimaryAttack"))
	NETVAR(CHandle<IClientEntity>, m_OwnerEntity, XorStr("DT_BaseEntity"), XorStr("m_hOwnerEntity"))
	NETVAR(float, NextPrimaryAttack, XorStr("DT_BaseCombatWeapon"), XorStr("LocalActiveWeaponData"), XorStr("m_flNextPrimaryAttack"))
	NETVAR(float, GetAccuracyPenalty, XorStr("DT_WeaponCSBase"), XorStr("m_fAccuracyPenalty"))
	NETVAR(float, GrenadeThrowTime, XorStr("DT_BaseCSGrenade"), XorStr("m_fThrowTime"))
	NETVAR(float, GetPostponeFireReadyTime, XorStr("DT_WeaponCSBase"), XorStr("m_flPostponeFireReadyTime"))
	NETVAR(float, GetThrowStrength, XorStr("DT_BaseCSGrenade"), XorStr("m_flThrowStrength"))
	NETVAR(short, GetItemDefinitionIndex, XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemDefinitionIndex"))
	NETVAR(int, GetRecoilIndex, XorStr("DT_WeaponCSBase"), XorStr("m_iRecoilIndex"))
	NETVAR(int, GetWorldModelIndex, XorStr("DT_BaseCombatWeapon"), XorStr("m_iWorldModelIndex"))
	NETVAR(int, EntityQuality, XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iEntityQuality"))
	NETVAR(int, GetClip1, XorStr("DT_BaseCombatWeapon"), XorStr("m_iClip1"))
	NETVAR(int, GetPrimaryReserveAmmoCount, XorStr("DT_BaseCombatWeapon"), XorStr("m_iPrimaryReserveAmmoCount"))
	NETVAR(int, GetState, XorStr("DT_BaseCombatWeapon"), XorStr("m_iState"))
	NETVAR(int, GetZoomLevel, XorStr("DT_WeaponCSBaseGun"), XorStr("m_zoomLevel"))
	NETVAR(CBaseHandle, GetWeaponWorldModel, XorStr("DT_BaseCombatWeapon"), XorStr("m_hWeaponWorldModel"))

#pragma region vfuncs
	const char* GetName()
	{
		typedef const char* (__thiscall* tGetName)(void*);
		return CallVFunction<tGetName>(this, 376)(this);
	}

	const CCSWeaponInfo* GetCSWpnData()
	{
		if (!this)
			return nullptr;

		return WEAPONSYSTEM->GetWpnData(GetItemDefinitionIndex());

		//typedef CCSWeaponInfo*(__thiscall* OriginalFn)(void*);
		//return CallVFunction<OriginalFn>(this, 447)(this);
	}

	float GetInaccuracy()
	{
		if (!this)
			return 0.f;

		typedef float(__thiscall* OriginalFn)(void*);
		return CallVFunction<OriginalFn>(this, 468)(this);
	}

	float GetSpread()
	{
		if (!this)
			return 0.f;

		typedef float(__thiscall* OriginalFn)(void*);
		return CallVFunction<OriginalFn>(this, 437)(this);
	}

	void UpdateAccuracyPenalty()
	{
		typedef void(__thiscall* OriginalFn)(void*);
		CallVFunction<OriginalFn>(this, 469)(this);
	}
#pragma endregion

	char GetWeaponIcon()
	{
		switch (GetId())
		{
		case weapon_ak47: return 'B';
		case weapon_tec9:
		case weapon_glock: return 'C';
		case weapon_mp9:
		case weapon_bizon: return 'D';
		case weapon_aug: return 'E';
		case weapon_revolver:
		case weapon_deagle: return 'F';
		case weapon_flashbang:
		case weapon_decoy: return 'G';
		case weapon_hegrenade:
		case weapon_molotov:
		case weapon_incgrenade: return 'H';
		case weapon_g3sg1:
		case weapon_scar20: return 'I';
		case weapon_knife:
		case weapon_knife_t:
		case weapon_knife_bayonet:
		case weapon_knife_flip:
		case weapon_knife_gut:
		case weapon_knife_karambit:
		case weapon_knife_m9_bayonet:
		case weapon_knife_tactical:
		case weapon_knife_falchion:
		case weapon_knife_survival_bowie:
		case weapon_knife_butterfly:
		case weapon_knife_push: return 'J';
		case weapon_mag7:
		case weapon_sawedoff:
		case weapon_nova: return 'K';
		case weapon_mac10: return 'L';
		case weapon_p90: return 'M';
		case weapon_ssg08: return 'N';
		case weapon_smokegrenade: return 'P';
		case weapon_ump: return 'Q';
		case weapon_awp: return 'R';
		case weapon_elite: return 'S';
		case weapon_famas: return 'T';
		case weapon_fiveseven: return 'U';
		case weapon_galilar: return 'V';
		case weapon_m4a4:
		case weapon_m4a1_silencer: return 'W';
		case weapon_mp7: return 'X';
		case weapon_taser:
		case weapon_hkp2000:
		case weapon_p250:
		case weapon_usp_silencer:
		case weapon_cz75a: return 'Y';
		case weapon_negev:
		case weapon_m249: return 'Z';
		case weapon_sg553: return '[';
		case weapon_xm1014: return ']';
		case weapon_c4:
		default: return '\\';
		}
	}

	IClientEntity* GetOwner() const
	{
		return reinterpret_cast<IClientEntity*>(ENTITYLIST->GetClientEntityFromHandle(m_OwnerEntity()));
	}

	int GetId()
	{
		int id = GetItemDefinitionIndex();

		switch (id)
		{
		case weapon_hegrenade:
		{
			const char* networkname = GetClientClass()->m_pNetworkName;

			if (strstr(networkname, XorStr("CHEGrenade"))) return weapon_hegrenade;
			if (strstr(networkname, XorStr("CFlashbang"))) return weapon_flashbang;
			if (strstr(networkname, XorStr("CSmokeGrenade"))) return weapon_smokegrenade;
			if (strstr(networkname, XorStr("CIncendiaryGrenade"))) return weapon_molotov;
			if (strstr(networkname, XorStr("CDecoyGrenade"))) return weapon_decoy;
		}

		default: return id;
		}
	}

	bool IsKnife()
	{
		auto weapon = EItemDefinitionIndex(GetId());
		if (weapon == weapon_knife || weapon == weapon_knife_t ||
			weapon == weapon_knife_bayonet || weapon == weapon_knife_flip ||
			weapon == weapon_knife_gut || weapon == weapon_knife_karambit ||
			weapon == weapon_knife_m9_bayonet || weapon == weapon_knife_tactical ||
			weapon == weapon_knife_falchion || weapon == weapon_knife_survival_bowie ||
			weapon == weapon_knife_butterfly || weapon == weapon_knife_push ||
			weapon == weapon_none)
			return true;

		return false;
	}

	bool IsGrenade()
	{
		EItemDefinitionIndex weapon = EItemDefinitionIndex(GetId());

		if (weapon == weapon_hegrenade || weapon == weapon_flashbang || weapon == weapon_incgrenade || weapon == weapon_molotov || weapon == weapon_decoy || weapon == weapon_smokegrenade)
			return true;

		return false;
	}

	WeaponType GetWeaponType()
	{
		if (!this)
			return WEAPONTYPE_OTHER;


		EItemDefinitionIndex weapon = EItemDefinitionIndex(GetId());

		if (weapon == weapon_deagle || weapon == weapon_elite || weapon == weapon_fiveseven || weapon == weapon_glock || weapon == weapon_p250 || weapon == weapon_hkp2000 || weapon == weapon_tec9 || weapon == weapon_taser || weapon == weapon_usp_silencer || weapon == weapon_cz75a/* || weapon == weapon_revolver*/)
			return WEAPONTYPE_PISTOL;
		if (weapon == weapon_mac10 || weapon == weapon_p90 || weapon == weapon_ump || weapon == weapon_bizon || weapon == weapon_mp7 || weapon == weapon_mp9)
			return WEAPONTYPE_SMG;
		if (weapon == weapon_xm1014 || weapon == weapon_mag7 || weapon == weapon_sawedoff || weapon == weapon_nova)
			return WEAPONTYPE_SHOTGUN;
		if (weapon == weapon_ak47 || weapon == weapon_aug || weapon == weapon_famas || weapon == weapon_galilar || weapon == weapon_m4a4 || weapon == weapon_sg553 || weapon == weapon_m4a1_silencer)
			return WEAPONTYPE_RIFLE;
		if (weapon == weapon_m249 || weapon == weapon_negev)
			return WEAPONTYPE_HEAVY;
		if (weapon == weapon_awp || weapon == weapon_ssg08 || weapon == weapon_scar20 || weapon == weapon_g3sg1)
			return WEAPONTYPE_SNIPER;
		if (weapon == weapon_knifegg || weapon == weapon_knife || weapon == weapon_flashbang || weapon == weapon_hegrenade || weapon == weapon_smokegrenade || weapon == weapon_molotov || weapon == weapon_incgrenade || weapon == weapon_decoy || weapon == weapon_c4 || weapon == weapon_knife_t || weapon == weapon_knife_bayonet || weapon == weapon_knife_flip || weapon == weapon_knife_gut || weapon == weapon_knife_karambit || weapon == weapon_knife_m9_bayonet || weapon == weapon_knife_tactical || weapon == weapon_knife_falchion || weapon == weapon_knife_survival_bowie || weapon == weapon_knife_butterfly || weapon == weapon_knife_push || weapon == weapon_none)
			return WEAPONTYPE_OTHER;
		if (weapon == weapon_revolver)
			return WEAPONTYPE_REVOLVER;


		return WEAPONTYPE_OTHER;
	}

	bool IsNadeThrown(float curtime)
	{
		if (!IsGrenade())
			return false;

		if (!GrenadeThrowTime())
			return false;

		return GrenadeThrowTime() <= curtime;
	}
};

class CBaseAttributableItem : public CBaseCombatWeapon {
public:
	NETVAR(float, GetFallbackWear, XorStr("DT_BaseAttributableItem"), XorStr("m_flFallbackWear"))
	NETVAR(int, GetItemIDHigh, XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemIDHigh"))
	NETVAR(int, GetItemIDLow, XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemIDLow"))
	NETVAR(int, GetEntityQuality, XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iEntityQuality"))
	NETVAR(int, GetAccountID, XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iAccountID"))
	NETVAR(unsigned, GetFallbackPaintKit, XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackPaintKit"))
	NETVAR(unsigned, GetFallbackSeed, XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackSeed"))
	NETVAR(unsigned, GetFallbackStatTrak, XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackStatTrak"))
	using str_32 = char[32];
	NETVAR(str_32, GetCustomName, XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_szCustomName"))
};

class CGlove : public CBaseCombatWeapon
{
public:
	void pSetModelIndex(int index)
	{
		typedef void(__thiscall* Fn)(void*, int);
		CallVFunction<Fn>(this, 75)(this, index);
	}

	void DataUpdate(int)
	{
		typedef void(*oPreDataUpdate)(void*, int);
		CallVFunction<oPreDataUpdate>(this, 6)(this + 0x8, 0);
	}
};

class CBaseViewModel : public CBaseEntity 
{
public:
	NETVAR(DWORD, GetOwner, XorStr("DT_BaseViewModel"), XorStr("m_hOwner"))
	NETVAR(DWORD, GetWeapon, XorStr("DT_BaseViewModel"), XorStr("m_hWeapon"))
};

class C_BaseWeaponWorldModel : public CBaseEntity
{
};

class C_CBaseCSGrenadeProjectile : public IClientEntity
{
public:
	NETVAR(Vector, GetInitialVelocity, XorStr("DT_BaseCSGrenadeProjectile"), XorStr("m_vInitialVelocity"))
};

class Player
{
public:
	Vector	backupangle;

	struct Animations
	{
		C_CSGOPlayerAnimState* anim_state;
		CBaseHandle m_ulEntHandle;
		float m_flSpawnTime;
		C_CSGOPlayerAnimState* GetAnimState(C_CSPlayer* ent);

	} Animations;

	struct
	{
		int nResolveTypePitch = 0;
		bool ResolveYaw = false;
		bool Bodyaim = false;
		bool bWhiteListed = false;
		bool bAimThisShot = false;
		bool bIsLegit = false;
	} corrections;

	bool overridden = false;
	float overridevalue = 0.f;
};

struct C_CSGOPlayerAnimState
{
	void UpdateAnimationState(QAngle &angle);
	void ResetAnimationState();
	void CreateAnimationState(C_CSPlayer * ent);
	void DebugShowAnimState();

	char pad[4];
	char bUnknown; //0x4
	char pad2[91];
	C_CSPlayer* pBaseEntity; //0x60
	CBaseCombatWeapon* pActiveWeapon; //0x64
	CBaseCombatWeapon* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flTimeDelta; //0x74
	float m_flEyeYaw; //0x78
	float m_flEyePitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	Vector m_vVelocity; //0xC8
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float m_velocity; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[532];
};
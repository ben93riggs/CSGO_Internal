#include "C_CSPlayer.h"
#include "Globals.h"
#include "Utils.hpp"

C_CSPlayer* C_CSPlayer::GetLocalPlayer()
{
	return (C_CSPlayer*)ENTITYLIST->GetClientEntity(ENGINE->GetLocalPlayer());
}

C_CSPlayer* C_CSPlayer::GetPlayerByIndex(int i_target_idx)
{
	return (C_CSPlayer*)ENTITYLIST->GetClientEntity(i_target_idx);
}

float_t& C_CSPlayer::m_surfaceFriction()
{
	static unsigned int _m_surfaceFriction = FindInDataMap(GetPredDescMap(), XorStr("m_surfaceFriction"));
	return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
}

Vector& C_CSPlayer::m_vecBaseVelocity()
{
	static unsigned int _m_vecBaseVelocity = FindInDataMap(GetPredDescMap(), XorStr("m_vecBaseVelocity"));
	return *(Vector*)((uintptr_t)this + _m_vecBaseVelocity);
}

float_t& C_CSPlayer::m_flMaxspeed()
{
	static unsigned int _m_flMaxspeed = FindInDataMap(GetPredDescMap(), XorStr("m_flMaxspeed"));
	return *(float_t*)((uintptr_t)this + _m_flMaxspeed);
}

VarMapping_t* C_CSPlayer::VarMapping()
{
	return (VarMapping_t*)((DWORD)this + 0x24);
}

void C_CSPlayer::SetAbsAngles( QAngle angles )
{
	//0xD0
	angles.x = 0;
	static auto fnsetabsangles = Utils::FindPattern(Utils::get_client_dll(), XorStr("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1"));
	reinterpret_cast<void(__thiscall*)(IClientEntity*, const QAngle&)>(fnsetabsangles)(this, angles);
}

void C_CSPlayer::SetAbsOrigin(const Vector &origin)
{
	//0xAC
	static auto fnsetabsorigin = Utils::FindPattern(Utils::get_client_dll(), XorStr("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));

	reinterpret_cast<void(__thiscall*)(IClientEntity*, const Vector&)>(fnsetabsorigin)(this, origin);
}

Vector &C_CSPlayer::GetAbsOrigin()
{
	return *(Vector*)((DWORD)this + 0xAC); //0xAC
}

QAngle &C_CSPlayer::GetAbsAngles()
{
	return *(Vector*)((DWORD)this + 0xD0); //0xD0
}

bool C_CSPlayer::IsPlayer()
{
	ClientClass* client_class = this->GetClientClass();

	if (!client_class)
		return false;

	return client_class->m_ClassID == CCSPlayer;
}

CBaseCombatWeapon* C_CSPlayer::GetActiveWeapon()
{
	static DWORD activeWeaponOff = GET_NETVAR(XorStr("DT_BaseCombatCharacter"), XorStr("m_hActiveWeapon"));
	assert(activeWeaponOff);
	return reinterpret_cast<CBaseCombatWeapon*>(ENTITYLIST->GetClientEntityFromHandle(GetFieldValue<CHandle<CBaseCombatWeapon>>(activeWeaponOff)));
}

std::string C_CSPlayer::sGetName() const
{
	player_info_t thorwhat;
	ENGINE->GetPlayerInfo(EntIndex(), &thorwhat);

	return thorwhat.m_szPlayerName;
}

//returns true if the player is reloading
bool C_CSPlayer::IsInReload()
{
	CBaseCombatWeapon* weapon = this->GetActiveWeapon();

	if (!weapon)
		return true;

	if (weapon->GetClip1() < 1)
		return true;

	float curTime = this->GetTickBase() * GLOBALS->interval_per_tick;

	return curTime < this->GetNextAttack();
}

bool C_CSPlayer::bCanFire()
{
	if (!this)
		return false;

	CBaseCombatWeapon* weapon = this->GetActiveWeapon();

	if (!weapon)
		return false;

	float server_time = this->GetTickBase() * GLOBALS->interval_per_tick;

	if (IsInReload())
		return false;

 	if (weapon->GetItemDefinitionIndex() == weapon_revolver)
 	{
		if (server_time < weapon->GetPostponeFireReadyTime())
			return false;
 	}

	//static auto nextprimary = weapon->NextPrimaryAttack();
	//if (nextprimary < weapon->NextPrimaryAttack())
	//	nextprimary = weapon->NextPrimaryAttack();
	//	
	//if (curTime < nextprimary)
	//	return false;
	//
	//INetChannelInfo *nci = ENGINE->GetNetChannelInfo();
	//nextprimary += nci->GetAvgLatency(2);
	if (server_time < weapon->NextPrimaryAttack())
		return false;
	return true;
}

bool C_CSPlayer::IsVisible(Vector &bonepos)
{
	auto pLocal = GetLocalPlayer();

	if (!pLocal)
		return false;

	Vector localEyePos = pLocal->GetEyePos();
	return Utils::IsVisibleOld(localEyePos, bonepos, pLocal);
}

//requires logging anim data in FSN but seems like the method of doing this in g2.1 is wrong
bool C_CSPlayer::HandleBoneSetup(int32_t boneMask, matrix3x4_t *boneOut, float_t curtime)
{
	studiohdr_t* hdr = MODELINFO->GetStudioModel(this->GetModel());
	if (!hdr) return false;

	CBoneAccessor *accessor = this->GetBoneAccessor();
	if (!accessor) return false;

	matrix3x4_t *backup_matrix = accessor->GetBoneArrayForWrite();
	if (!backup_matrix) return false;

	Vector origin = this->GetOrigin();
	QAngle angles = this->GetEyeAngles();

	Vector backup_origin = this->GetAbsOrigin();
	QAngle backup_angles = this->GetAbsAngles();

	//std::array<float, 24> backup_poses = this->GetPoseParameters();

	C_AnimationLayer backup_layers[15];
	memcpy(backup_layers, this->GetAnimOverlays(), (sizeof(C_AnimationLayer) * this->GetNumAnimOverlays()));

	alignas(16) matrix3x4_t parentTransform;
	Math::AngleMatrix(angles, origin, parentTransform);

	//auto &anim_data = Animation::get().GetPlayerAnimationInfo(this->EntIndex());

	//this->GetAbsOrigin() = origin;
	//this->GetAbsAngles() = angles;
	//this->GetPoseParameters() = anim_data.m_flPoseParameters;
	//memcpy(this->GetAnimOverlays(), anim_data.m_AnimationLayer, (sizeof(C_AnimationLayer) * this->GetNumAnimOverlays()));

	Vector *pos = (Vector*)(MEMALLOC->Alloc(sizeof(Vector[128])));
	Quaternion *q = (Quaternion*)(MEMALLOC->Alloc(sizeof(Quaternion[128])));
	memset(pos, 0xFF, sizeof(pos));
	memset(q, 0xFF, sizeof(q));

	this->StandardBlendingRules(hdr, pos, q, curtime, boneMask);

	accessor->SetBoneArrayForWrite(boneOut);

	byte *computed = (byte*)(MEMALLOC->Alloc(sizeof(byte[0x20])));
	memset(computed, 0, sizeof(byte[0x20]));

	this->BuildTransformations(hdr, pos, q, parentTransform, boneMask, &computed[0]);

	accessor->SetBoneArrayForWrite(backup_matrix);

	//this->GetAbsOrigin() = backup_origin;
	//this->GetAbsAngles() = backup_angles;
	//this->GetPoseParameters() = backup_poses;
	memcpy(this->GetAnimOverlays(), backup_layers, (sizeof(C_AnimationLayer) * this->GetNumAnimOverlays()));

	return true;
}

void C_CSPlayer::InvalidatePhysicsRecursive(int nChangeFlags)
{
	return reinterpret_cast<void(__thiscall*)(C_CSPlayer*, int)>(Interface->InvalidatePhysicsRecursive())(this, nChangeFlags);
}

CBoneAccessor* C_CSPlayer::GetBoneAccessor() 
{
	static uint16_t offset = *(uint16_t*)(Utils::FindSignature(Utils::get_client_dll(), XorStr("8D 81 ?? ?? ?? ?? 50 8D 84 24")) + 2);

	if (!offset)
		return nullptr;

	return (CBoneAccessor*)((uintptr_t)this + offset);
}

void C_CSPlayer::InvalidateBoneCache()
{
	assert(Interface->InvalidateBoneCache());
	unsigned long g_iModelBoneCounter = *(unsigned long*)(Interface->InvalidateBoneCache() + 18);
	*(float*)((DWORD)this + 0x2914) = -FLT_MAX;												            // m_flLastBoneSetupTime = -FLT_MAX;
	*(unsigned int*)((DWORD)this + 0x2680) = (g_iModelBoneCounter - 1);									// m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}

//on second thought having an override here may be a bad idea as it would change ecx to IClientEntity instead of IClientRenderable (player + 4)
bool C_CSPlayer::SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	if (!this)
		return false;

	if ((uintptr_t)this <= 0x1000)
		return false;

	if (this == GetLocalPlayer())
		return oSetupBones(pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	*(int*)((uintptr_t)this + 0xA30) = GLOBALS->framecount; //skip occlusion checks
	*(int*)((uintptr_t)this + 0xA28) = 0; //clear occlusion flags

	bool bret = oSetupBones(pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	return bret;
}

void C_CSPlayer::UpdateIKLocks(float currentTime)
{
	typedef void(__thiscall* oUpdateIKLocks)(PVOID, float currentTime);
	CallVFunction<oUpdateIKLocks>(this, 186)(this, currentTime);
}

void C_CSPlayer::CalculateIKLocks(float currentTime)
{
	typedef void(__thiscall* oCalculateIKLocks)(PVOID, float);
	CallVFunction<oCalculateIKLocks>(this, 187)(this, currentTime);
}

void C_CSPlayer::StandardBlendingRules(studiohdr_t* pStudioHdr, Vector* pos, Quaternion* q, float currentTime, int boneMask)
{
	typedef void(__thiscall* oStandardBlendingRules)(PVOID, studiohdr_t*, Vector*, Quaternion*, float, int);
	CallVFunction<oStandardBlendingRules>(this, 200)(this, pStudioHdr, pos, q, currentTime, boneMask);
}

void C_CSPlayer::ControlMouth(studiohdr_t* pStudioHdr)
{
	typedef void(__thiscall* oControlMouth)(PVOID, studiohdr_t*);
	CallVFunction<oControlMouth>(this, 191)(this, pStudioHdr);
}

void C_CSPlayer::BuildTransformations(studiohdr_t* pStudioHdr, Vector* pos, Quaternion* q, const matrix3x4_t& cameraTransform, int boneMask, byte* computed)
{
	typedef void(__thiscall* oStandardBlendingRules)(PVOID, studiohdr_t*, Vector*, Quaternion*, const matrix3x4_t&, int, byte*);
	CallVFunction<oStandardBlendingRules>(this, 184)(this, pStudioHdr, pos, q, cameraTransform, boneMask, computed);
}

bool C_CSPlayer::IsAlive() const
{
	return m_lifeState() == 0;
}

//should be replaced with below
int C_CSPlayer::GetChokedPackets()
{
	if (!this)
		return 0;

	return g::choked_packets[EntIndex()];
}

int C_CSPlayer::GetNumberOfTicksChoked() const
{
	float flSimulationTime = GetSimulationTime();      // last time entity was updated on the server
	float latency = Utils::GetNetworkLatency();
	float serverTime = GLOBALS->curtime - latency; // current server time
	float flSimDiff = serverTime - flSimulationTime;
	return TIME_TO_TICKS(max(0.0f, flSimDiff - latency));
}

Vector C_CSPlayer::GetEyePos() const
{
	return GetOrigin() + GetViewOffset();
}

std::array<float, 24>& C_CSPlayer::GetPoseParameters()
{
	static DWORD poseParamOff = GET_NETVAR(XorStr("DT_BaseAnimating"), XorStr("m_flPoseParameter"));
	assert(poseParamOff);
	return *(std::array<float, 24>*)((uintptr_t)this + poseParamOff);
}

void C_CSPlayer::SetPoseAngles(float_t yaw, float_t pitch)
{
	auto &poses = GetPoseParameters();
	poses[11] = (pitch + 90) / 180;
	poses[2] = (yaw + 180) / 360;
}

void C_CSPlayer::SetPoseParameters(std::array<float, 24> &arr)
{
	static DWORD poseParamOff;

	if (!poseParamOff)
		poseParamOff = GET_NETVAR(XorStr("DT_BaseAnimating"), XorStr("m_flPoseParameter"));

	*(std::array<float, 24>*)(reinterpret_cast<DWORD>(this) + poseParamOff) = arr;
}

float C_CSPlayer::GetBombTimer()
{
	static DWORD bombTimeOff;

	if (!bombTimeOff)
		bombTimeOff = GET_NETVAR(XorStr("DT_PlantedC4"), XorStr("m_flC4Blow"));

	float bombTime = GetFieldValue<float>(bombTimeOff);
	float returnValue = bombTime - GLOBALS->curtime;
	return (returnValue < 0) ? 0.f : returnValue;
}

void C_CSPlayer::ExtrapolateTick(Vector& p0) const
{
	p0 += this->GetVelocity() * GLOBALS->interval_per_tick;
}

void C_CSPlayer::ExtrapolateChokedTicks()
{
	Vector velocity = GetVelocity();
	int choked_ticks = g::choked_packets[EntIndex()];
	float extrapolated_tick_time = choked_ticks * GLOBALS->interval_per_tick;

	SetAbsOrigin(GetOrigin() + velocity * extrapolated_tick_time);
	GetSimulationTime() = GetSimulationTime() + choked_ticks;
}

UINT C_CSPlayer::PhysicsSolidMaskForEntity()
{
	typedef UINT(__thiscall* OriginalFn)(void*);
	return CallVFunction<OriginalFn>(this, 148)(this);
}

void C_CSPlayer::UpdateClientSideAnimation()
{
	//static uintptr_t UpdateClientsideAnimationIndex;

	//if (!UpdateClientsideAnimationIndex)
	//	UpdateClientsideAnimationIndex = *(uintptr_t*)(Utils::FindSignature(Utils::get_client_dll(), XorStr("74 0B 8B 0C F0 8B 01 FF 90")) + 0x9) / 4; //divide by four to find vfunc index

	//if (!this || !UpdateClientsideAnimationIndex)
	//	return;

	CallVFunction<void(__thiscall*)(void*)>(this, 218)(this);
}

C_AnimationLayer* C_CSPlayer::GetAnimOverlays()
{
	return *(C_AnimationLayer**)((DWORD)this + 0x2970); //55 8B EC 51 53 8B 5D 08 33 C0
}

C_AnimationLayer& C_CSPlayer::GetAnimOverlay2(int Index)
{
	return (*(C_AnimationLayer**)((DWORD)this + 0x2970))[Index]; //55 8B EC 51 53 8B 5D 08 33 C0
}

std::array<C_AnimationLayer, 13> C_CSPlayer::GetAnimOverlay()
{
	return *reinterpret_cast<std::array<C_AnimationLayer, 13>*>(this + Interface->AnimOverlay()); //55 8B EC 51 53 8B 5D 08 33 C0
}

C_AnimationLayer *C_CSPlayer::GetAnimOverlay(int i)
{
	if (i < 15)
		return &GetAnimOverlays()[i];

	return nullptr;
}

int C_CSPlayer::GetSequenceActivity(int sequence)
{
	auto hdr = MODELINFO->GetStudioModel(this->GetModel());

	if (!hdr)
		return -1;

	// c_csplayer vfunc 242, follow calls to find the function.

	// (DWORD)Utils::PatternScan(client, "55 8B EC 83 7D 08 FF 56 8B F1 74 3D")
	static auto GetSequenceActivity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(Interface->SequenceActivity());

	return GetSequenceActivity(this, hdr, sequence);
}

void C_CSGOPlayerAnimState::UpdateAnimationState(QAngle &angle)
{
	typedef void(__vectorcall *fnUpdateAnimState)(PVOID, PVOID, float, float, float, PVOID);
	static auto UpdateAnimState = Utils::FindPattern(Utils::get_client_dll(), XorStr("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));

	if (!UpdateAnimState || (this == nullptr))
		return;

	((fnUpdateAnimState)(UpdateAnimState))(this, nullptr, NULL, angle.y, angle.x, nullptr);
}

void C_CSGOPlayerAnimState::ResetAnimationState()
{
	if (!this)
		return;

	using ResetAnimState_t = void(__thiscall*)(C_CSGOPlayerAnimState*);
	static auto ResetAnimState = Utils::FindPattern(Utils::get_client_dll(), XorStr("56 6A 01 68 ? ? ? ? 8B F1"));

	if (!ResetAnimState || (this == nullptr))
		return;

	((ResetAnimState_t)(ResetAnimState))(this);
}

void C_CSGOPlayerAnimState::CreateAnimationState(C_CSPlayer* ent)
{
	using CreateAnimState_t = void(__thiscall*)(C_CSGOPlayerAnimState*, C_CSPlayer*);
	static auto CreateAnimState = Utils::FindPattern(Utils::get_client_dll(), XorStr("55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"));

	if (!CreateAnimState || (this == nullptr))
		return;

	((CreateAnimState_t)(CreateAnimState))(this, ent);
}

void C_CSGOPlayerAnimState::DebugShowAnimState()
{
	using CreateAnimState_t = void(__thiscall*)(C_CSGOPlayerAnimState*, unsigned int a2);
	static auto DebugShowAnimState = Utils::FindPattern(Utils::get_client_dll(), XorStr("55 8B EC 83 E4 C0 83 EC 34 53 56 8B F1 57 80"));

	if (!DebugShowAnimState || (this == nullptr))
		return;

	((CreateAnimState_t)(DebugShowAnimState))(this, 10);
}

C_CSGOPlayerAnimState* Player::Animations::GetAnimState(C_CSPlayer * ent)
{
	if (CLIENTSTATE->m_nDeltaTick == -1 || globals::get().full_update)
		return nullptr;

	// free old animstate
	if (anim_state && ent->GetRefEHandle() != m_ulEntHandle)
	{
		MEMALLOC->Free(anim_state);
		anim_state = nullptr;
	}

	// player changed / is nullptr
	if (!anim_state)
	{
		auto state = static_cast<C_CSGOPlayerAnimState*>(MEMALLOC->Alloc(sizeof(C_CSGOPlayerAnimState)));

		if (state != nullptr)
			state->CreateAnimationState(ent);

		m_ulEntHandle = ent->GetRefEHandle();
		m_flSpawnTime = ent->GetSpawnTime();

		anim_state = state;
	}
	else if (ent->GetSpawnTime() != m_flSpawnTime)
	{
		anim_state->ResetAnimationState();
		m_flSpawnTime = ent->GetSpawnTime();
	}

	return anim_state;
}

bool C_CSPlayer::IsBreakableEnt()
{
	if (!this)
		return false;

	bool bRet = false;

	if (this != ENTITYLIST->GetClientEntity(0))
	{
		static DWORD dwEntBreakable = NULL;

		if (!dwEntBreakable)
			dwEntBreakable = Utils::FindPattern(Utils::get_client_dll(), (PBYTE)XorStr("\x55\x8B\xEC\x51\x56\x8B\xF1\x85\xF6\x74\x68"), XorStr("xxxxxxxxxxx"));

		if (!dwEntBreakable)
			return false;

		int DAMAGE_YES_BACKUP = *this->m_takedamage();
		*this->m_takedamage() = DAMAGE_YES;

		__asm
		{
			MOV ECX, this
			CALL dwEntBreakable
			MOV bRet, AL
		}

		*this->m_takedamage() = DAMAGE_YES_BACKUP;

	}
	

	return bRet;
}

bool C_CSPlayer::GetHitboxPosition(int hitbox_num, matrix3x4_t* matrix, Vector& out)
{
	studiohdr_t* hdr = MODELINFO->GetStudioModel(GetModel());

	if (!hdr)
		return false;

	mstudiohitboxset_t* set = hdr->GetHitboxSet(GetPlayerHitboxSet());

	if (!set)
		return false;

	mstudiobbox_t* hitbox = set->pHitbox(hitbox_num);

	if (!hitbox)
		return false;

	Math::VectorTransform((hitbox->bbmin + hitbox->bbmax) * .5f, matrix[hitbox->bone], out);
	return true;
}
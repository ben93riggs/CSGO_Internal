#include "Accuracy.h"
#include "C_CSPlayer.h"
#include "Globals.h"
#include "RageBot.h"
#include "DeveloperMessages.h"
#include <cfloat>
#include <float.h>

bool is_fakewalking(C_CSPlayer *pEnt) {
	const auto anim_layer = pEnt->GetAnimOverlay(12);
	const auto velocity = pEnt->GetVelocity().Length2D();
	if (anim_layer->m_flWeight >= 0.01f)
		return false;
	return (velocity > .1f) && (velocity < 60.f);
}

//ctor
Record::Record(C_CSPlayer *pEntity, BacktrackedPlayer &Player) {
	m_flSimulationTime = Player.m_flSimulationTime;
	m_nFlags = Player.m_nFlags;
	m_nSequence = Player.m_nSequence;
	m_flCycle = Player.m_flCycle;
	m_vecOrigin = Player.m_vecOrigin;
	m_angAngles = Player.m_angAngles;
	m_vecMins = Player.m_vecMins;
	m_vecMaxs = Player.m_vecMaxs;
	m_flLowerBodyYawTarget = Player.m_flLowerBodyYawTarget;
	m_nLbyUpdated = Player.m_nLbyUpdated;
	m_arrflPoseParameters = Player.m_arrflPoseParameters;
	m_LayerRecords = Player.m_LayerRecords;

	if (MenuVars::get().g_bResolveYaw) {
		bool nospread = false;
		if (nospread) {
			const int pitch = (int)floor(Player.m_nBruteforce / 8.f);
			static const int pitches[3] = { 89, -89, 0 };
			m_angAngles.x = pitches[pitch];
		}
		if (m_nLbyUpdated) {
			m_angAngles.y = m_flLowerBodyYawTarget + ((m_nLbyUpdated == 2) ? 35 * Player.m_nLbyCorrect : 0);
		}
		else {
			m_angAngles.y = m_flLowerBodyYawTarget;
			static const int yaws[8] = { 180, 0, 90, -90, 45, -135, 135, -45 };
			const int yaw = Player.m_nBruteforce % 8;
			m_angAngles.y += yaws[yaw];
		}

	}

	auto old_curtime = GLOBALS->curtime;
	auto old_frametime = GLOBALS->frametime;

	GLOBALS->curtime = pEntity->GetSimulationTime();
	GLOBALS->frametime = GLOBALS->interval_per_tick;
	//C_CSGOPlayerAnimState *anim_state = g::plist[pEntity->EntIndex()].Animations.GetAnimState( pEntity );
	C_CSGOPlayerAnimState *anim_state = pEntity->GetPlayerAnimState();
	if( anim_state ) {
		anim_state->m_flLastClientSideAnimationUpdateTime = 0;
		anim_state->m_iLastClientSideAnimationUpdateFramecount = 0;
		pEntity->GetEyeAngles() = m_angAngles;
		pEntity->m_bClientSideAnimation() = true;
		//anim_state->UpdateAnimationState( m_angAngles );
		pEntity->UpdateClientSideAnimation();
		pEntity->m_bClientSideAnimation() = false;
	}
	GLOBALS->curtime = old_curtime;
	GLOBALS->frametime = old_frametime;


	pEntity->SetAbsAngles( m_angAngles );
	pEntity->SetAbsOrigin( m_vecOrigin );
	pEntity->InvalidateBoneCache();
	m_bMatrixBuilt = pEntity->SetupBones( m_BoneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.f );
	m_vecHeadSpot = Utils::GetHitboxPosition( pEntity, HITBOX_HEAD );
	pEntity->SetAbsAngles( Player.m_angAbsAngles );
	pEntity->SetAbsOrigin( Player.m_vecAbsOrgin );
}

float Accuracy::GetLerpTime() {
	int ud_rate = CVAR->FindVar(XorStr("cl_updaterate"))->GetInt();
	ConVar *min_ud_rate = CVAR->FindVar(XorStr("sv_minupdaterate"));
	ConVar *max_ud_rate = CVAR->FindVar(XorStr("sv_maxupdaterate"));

	if (min_ud_rate && max_ud_rate)
		ud_rate = max_ud_rate->GetInt();

	float ratio = CVAR->FindVar(XorStr("cl_interp_ratio"))->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = CVAR->FindVar(XorStr("cl_interp"))->GetFloat();
	ConVar *c_min_ratio = CVAR->FindVar(XorStr("sv_client_min_interp_ratio"));
	ConVar *c_max_ratio = CVAR->FindVar(XorStr("sv_client_max_interp_ratio"));

	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1)
		ratio = Math::clamp(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());

	return max(lerp, (ratio / ud_rate));
}
/*if ( v39 )
            v40 = -1431655765 * ((v39 - *&gpGlobals[1].m_bClient) >> 3);
          v41 = (*(*engine + 168LL))(engine, v40); GetPlayerNetInfo
          if ( v41 )
            (*(*v41 + 72LL))(v41, 0LL);
          v42 = *(v77 + 3756) + 0.0;
          if ( qword_F79958 == &sv_maxunlag )
            LODWORD(v7) = _mm_cvtsi32_si128(dword_F79974 ^ &sv_maxunlag);
          else
            (*(*qword_F79958 + 120LL))();
          v43 = 0.0;
          if ( v42 >= 0.0 )
            v43 = fminf(v7, v42);
          v44 = gpGlobals->curtime;
          v69 = gpGlobals->interval_per_tick;
          v45 = (*(v82 + 12) * v69) - *(v77 + 3756);
          if ( COERCE_FLOAT(COERCE_UNSIGNED_INT((v45 - v44) + v43) & xmmword_A46E90) > 0.2 )
            v45 = v44 - v43;
          flTargetTimeb = v45;*/ // lets stop agruing about this now
bool Accuracy::is_tick_valid(const float time) {
	const auto nci = ENGINE->GetNetChannelInfo();

	if (!nci)
		return false;

	static float correct = 0;
	static float server_time = 0;
	static float last_time = -1;

	if(last_time != GLOBALS->curtime) {
		correct = Math::clamp<float>(nci->GetLatency(FLOW_OUTGOING) + GetLerpTime(), 0.f, 1.f/*sv_maxunlag*/);
		server_time = GLOBALS->curtime + nci->GetAvgLatency(FLOW_INCOMING) + TICKS_TO_TIME(CLIENTSTATE->chokedcommands + 1);
	}

	last_time = GLOBALS->curtime;

	const auto delta_time = (time - server_time) + correct;// time doesnt need the lerptime in it because we adjust//correct - (server_time - (time - GetLerpTime())); // ((v45 - v44) + v43)

	return fabs(delta_time) <= 0.2f;
}

//for use in ESP so we dont just call back() and get some super old record. should always check if empty before calling this.
Record& Accuracy::GetLastValidRecord(int index) {
	std::vector<Record> &records = Players[index].m_LagRecord;

	int besttick = 0;
	for (auto i = 0; i > records.size(); i++) {
		if (!is_tick_valid(records[i].m_flSimulationTime))
			continue;
		
		besttick = i;
	}

	return records[besttick];
}

bool Accuracy::TraceToExitShort(Vector &point, Vector &dir, float flStepSize, float flMaxDistance)
{
	float flDistance = 0;

	while (flDistance <= flMaxDistance)
	{
		flDistance += flStepSize;

		point += dir * flDistance;

		if ((ENGINETRACE->GetPointContents(point) & MASK_SOLID) == 0)
		{
			// found first free point
			return true;
		}
	}

	return false;
}

float Accuracy::GetThickness(Vector &start, Vector &end) {
	Vector dir = end - start;
	Vector step = start;
	dir /= dir.Length();
	CTraceFilterWorldOnly filter;
	trace_t trace;
	Ray_t ray;
	float thickness = 0;
	while (true) {
		ray.Init(step, end);
		ENGINETRACE->TraceRay(ray, MASK_SOLID, &filter, &trace);

		if (!trace.DidHit())
			break;

		const Vector lastStep = trace.endpos;
		step = trace.endpos;

		if ((end - start).Length() <= (step - start).Length())
			break;

		if (!TraceToExitShort(step, dir, 5, 90))
			return FLT_MAX;

		thickness += (step - lastStep).Length();
	}
	return thickness;
}


bool Accuracy::Predict(C_CSPlayer *pEntity, BacktrackedPlayer &Player) {
	if (!pEntity) return false;
	if (Player.m_LagRecord.empty())
		return false;

	bool nospread = false;
	if (!nospread) return false;

	float delta_simtime = Player.m_flSimulationTime - Player.m_flPrevSimulationTime;
	Record &restore = Player.m_LagRecord[0];
	float predicted_simtime = restore.m_flSimulationTime + delta_simtime;
	Vector origin_difference = (pEntity->GetVelocity() * GLOBALS->interval_per_tick) * int(ceil(delta_simtime / GLOBALS->interval_per_tick));
	if (origin_difference.Length2DSqr() <= 4096.f || !is_tick_valid(predicted_simtime)) {
		return false;
	}

	Vector new_origin = restore.m_vecOrigin + origin_difference;

	const int layerCount = pEntity->GetNumAnimOverlays(); // restore layers before we calculate for new tick
	for (int i = 0; i < layerCount; i++)
	{
		C_AnimationLayer &currentLayer = pEntity->GetAnimOverlay2(i);
		currentLayer.m_nOrder = restore.m_LayerRecords[i].m_nOrder;
		currentLayer.m_nSequence = restore.m_LayerRecords[i].m_nSequence;
		currentLayer.m_flWeight = restore.m_LayerRecords[i].m_flWeight;
		currentLayer.m_flCycle = restore.m_LayerRecords[i].m_flCycle;
	}

	Vector angle = restore.m_angAngles;

	int pitch = (int)floor(Player.m_nBruteforce / 8.f);
	static const int pitches[8] = { 89, -89, 0 };
	angle.x = pitches[pitch];

	static const int angles[8] = { 180, 0, 90, -90, 45, -135, 135, -45 };
	angle.y += angles[Player.m_nBruteforce % 8];
	

	pEntity->GetSimulationTime() = predicted_simtime;
	pEntity->GetFlags() = restore.m_nFlags;

	pEntity->SetAbsAngles(angle);
	pEntity->SetAbsOrigin(new_origin);
	pEntity->GetEyeAngles() = angle;
	pEntity->GetOrigin() = new_origin;
	pEntity->GetCollideable()->OBBMaxs() = restore.m_vecMaxs;
	pEntity->GetCollideable()->OBBMins() = restore.m_vecMins;

	//auto old_curtime = GLOBALS->curtime;
	//auto old_frametime = GLOBALS->frametime;
	//
	//GLOBALS->curtime = pEntity->GetSimulationTime();
	//GLOBALS->frametime = GLOBALS->interval_per_tick;
	//C_CSGOPlayerAnimState *anim_state = g::plist[pEntity->EntIndex()].Animations.GetAnimState( pEntity );
	//C_CSGOPlayerAnimState *anim_state = pEntity->GetPlayerAnimState();
	//if (anim_state) {
	//	anim_state->m_flLastClientSideAnimationUpdateTime = 0;
	//	anim_state->m_iLastClientSideAnimationUpdateFramecount = 0;
	//	anim_state->UpdateAnimationState(angle);
	//	pEntity->UpdateClientSideAnimation();
	//}
	//GLOBALS->curtime = old_curtime;
	//GLOBALS->frametime = old_frametime;
	return true;
}

void Accuracy::Resolve(C_CSPlayer *pEntity, BacktrackedPlayer &Player) {
	if (!pEntity) return;

	if( Player.m_LagRecord.empty() )
		return;
	
	//if (Predict(pEntity, Player)) return;

	Record &restore = Player.m_LagRecord[0];
	CBoneAccessor* boneaccessor = pEntity->GetBoneAccessor();

	if (boneaccessor == nullptr || !restore.m_bMatrixBuilt) {
		std::cout << XorStr("bone invalid") << std::endl;
		return;
	}

	//if (boneaccessor != nullptr) {
	//	MEMALLOC->Free(boneaccessor->m_pBones);
	//	boneaccessor->m_pBones = static_cast<matrix3x4_t*>(MEMALLOC->Alloc(sizeof(restore.m_BoneMatrix)));
	//}
	//else {
	//	std::cout << XorStr("bones needed new alloc") << std::endl;
	//	boneaccessor->m_pBones = static_cast<matrix3x4_t*>(MEMALLOC->Alloc(sizeof(restore.m_BoneMatrix)));
	//}

	boneaccessor->m_ReadableBones = boneaccessor->m_WritableBones = MAXSTUDIOBONES;
	memcpy(boneaccessor->m_pBones, restore.m_BoneMatrix, MAXSTUDIOBONES * sizeof(matrix3x4_t));

	pEntity->GetSimulationTime() = restore.m_flSimulationTime;
	Player.m_bBacktracked = true;
	Player.m_bBacktrackedToLby = restore.m_nLbyUpdated;
}

bool BacktrackedPlayer::Cache(C_CSPlayer *pEntity) {
	if (!pEntity) return false;

	pEntity->m_bClientSideAnimation() = false;

	const bool ret = fabs(pEntity->GetSimulationTime() - m_flSimulationTime) != 0.f;

	if (ret) {
		m_nLbyUpdated = false;
		if (m_flLowerBodyYawTarget != pEntity->GetLowerBodyYawTarget()) {
			m_nLbyUpdated = 1;
			m_nLbyCorrect = 0;
		}
		if (pEntity->GetVelocity().Length2D() > 0.1f && !is_fakewalking(pEntity))
			m_nLbyUpdated = 1;

		if ((m_flLbyUpdate + 1.1f <= pEntity->GetSimulationTime()) && m_bLbyTimingAccurate) { // if our timing isnt correct we dont want the cheat thinking lby is updating
			m_flSimulationTime = m_flLbyUpdate;
			m_nLbyUpdated = 2;
		}

		if (m_nLbyUpdated != 0) {
			if (m_nLbyUpdated == 1)
				m_bLbyTimingAccurate = true; // received new and accurate timing
			m_flLbyUpdate = pEntity->GetSimulationTime();
		}
		m_flPrevSimulationTime = m_flSimulationTime;
	}

	m_flSimulationTime = pEntity->GetSimulationTime();
	m_nFlags = pEntity->GetFlags();
	m_angAngles = pEntity->GetEyeAngles();
	m_angAbsAngles = pEntity->GetAbsAngles();
	m_vecAbsOrgin = pEntity->GetAbsOrigin();
	m_vecOrigin = pEntity->GetOrigin();
	m_vecMaxs = pEntity->GetCollideable()->OBBMaxs();
	m_vecMins = pEntity->GetCollideable()->OBBMins();


	m_nSequence = pEntity->GetSequence();
	m_flCycle = pEntity->GetCycle();

	m_arrflPoseParameters = pEntity->GetPoseParameters();

	const int layerCount = pEntity->GetNumAnimOverlays();
	for (int i = 0; i < layerCount; i++)
	{
		C_AnimationLayer &currentLayer = pEntity->GetAnimOverlay2(i);
		m_LayerRecords[i].m_nOrder = currentLayer.m_nOrder;
		m_LayerRecords[i].m_nSequence = currentLayer.m_nSequence;
		m_LayerRecords[i].m_flWeight = currentLayer.m_flWeight;
		m_LayerRecords[i].m_flCycle = currentLayer.m_flCycle;
	}

	m_flLowerBodyYawTarget = pEntity->GetLowerBodyYawTarget();
	return ret;
}

bool Accuracy::MissedDueToSpread() {
	C_CSPlayer* pEntity = C_CSPlayer::GetPlayerByIndex(m_LastShot.m_nTargetIndex);
	if (!pEntity)
		return false;
	if (m_Impact == Vector(0,0,0))
		return false;
	CBoneAccessor* boneaccessor = pEntity->GetBoneAccessor();

	if (!boneaccessor) {
		std::cout << XorStr("boneaccessor invalid") << std::endl;
		return false;
	}

	Ray_t ray;
	trace_t trace;
	CTraceFilterPlayer filter;
	filter.pHit = pEntity;

	Vector end = m_Impact - m_LastShot.m_EyePos;
	end /= end.Length();
	end *= 8128.f;
	end += m_LastShot.m_EyePos;
	ray.Init(m_LastShot.m_EyePos, end);

	if (boneaccessor == nullptr) {
		std::cout << XorStr("bone invalid") << std::endl;
		return false;
	}

	//if (boneaccessor != nullptr)
	//	boneaccessor->m_pBones = static_cast<matrix3x4_t*>(MEMALLOC->Realloc(boneaccessor->m_pBones, sizeof(m_LastShot.m_BoneMatrix)));
	//else {
	//	std::cout << XorStr("bones needed new alloc") << std::endl;
	//	boneaccessor->m_pBones = static_cast<matrix3x4_t*>(MEMALLOC->Alloc(sizeof(m_LastShot.m_BoneMatrix)));
	//}

	boneaccessor->m_ReadableBones = m_LastShot.m_nReadableBones;
	boneaccessor->m_WritableBones = m_LastShot.m_nWritableBones;
	memcpy(boneaccessor->m_pBones, m_LastShot.m_BoneMatrix, MAXSTUDIOBONES * sizeof(matrix3x4_t));
	
	ENGINETRACE->TraceRay(ray, MASK_SHOT, &filter, &trace);
	return trace.m_pEnt == nullptr;
	if (VisualVars::get().ESP.g_bBulletTracer)
	{
		studiohdr_t* hdr = MODELINFO->GetStudioModel(pEntity->GetModel());

		if (hdr)
		{

			mstudiohitboxset_t* set = hdr->GetHitboxSet(pEntity->GetPlayerHitboxSet());

			if (set) {

				for (int i = 0; i < set->numhitboxes; i++)
				{
					mstudiobbox_t* hitbox = set->pHitbox(i);

					if (!hitbox)
						continue;

					Color col = Color::Blue();

					if (hitbox->radius == -1.0f)
					{
						Vector position;
						QAngle angles;
						Math::MatrixAngles(m_LastShot.m_BoneMatrix[hitbox->bone], angles, position);
						DEBUGOVERLAY->AddBoxOverlay(position, hitbox->bbmin, hitbox->bbmax, angles, col.GetR(), col.GetG(), col.GetB(), 255, 2.5f);
					}
					else
					{
						Vector vMin, vMax;
						Math::VectorTransformZ(hitbox->bbmin, m_LastShot.m_BoneMatrix[hitbox->bone], vMin);
						Math::VectorTransformZ(hitbox->bbmax, m_LastShot.m_BoneMatrix[hitbox->bone], vMax);
						DEBUGOVERLAY->DrawPill(vMin, vMax, hitbox->radius, col.GetR(), col.GetG(), col.GetB(), 255, 2.5f);
					}
				}
			}
		}
	}
}

void Accuracy::StorePlayers() {
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (m_bShotReceived) {
		if (m_LastShot.m_nTargetIndex != -1) {
			bool shouldBrute = false;
			if (m_nHurtIndex == -1)
				shouldBrute = true;
			else if (m_nHurtIndex != m_LastShot.m_nTargetIndex)
				shouldBrute = true;

			BacktrackedPlayer &Player = Players[m_LastShot.m_nTargetIndex];
			bool nospread = false;

			if(shouldBrute) 
			{
				if (!MissedDueToSpread()) 
				{
					if (m_LastShot.m_bLbyUpdate != 0) 
					{
						if (m_LastShot.m_bLbyUpdate == 2) 
						{
							Player.m_nLbyCorrect++;

							if (Player.m_nLbyCorrect > 1)
								Player.m_nLbyCorrect = -1;

							if (Player.m_nLbyCorrect == 0) // we've shot at every possible lby angle, our timing is off
								Player.m_bLbyTimingAccurate = false;
						}
					}
					else 
					{
						Player.m_nBruteforce++;
						if (Player.m_nBruteforce > (nospread ? 23 : 7))
							Player.m_nBruteforce = 0;
						if(!nospread)
							Player.m_bShouldBaim = true;
					}
				}
				else DevMsg::get().OutputMessage(Color::White(), XorStr("missed shot due to spread"));
			}
			else if (m_LastShot.m_bLbyUpdate == 0 && !nospread)
				Player.m_bShouldBaim = false;

			m_LastShot.m_nTargetIndex = -1;
		}
		m_nHurtIndex = -1;
		m_bShotReceived = false;
		m_Impact = Vector(0,0,0);
	}

	for (int i = 1; i < 64; i++)
	{
		C_CSPlayer* pEntity = C_CSPlayer::GetPlayerByIndex(i);

		BacktrackedPlayer &Player = Players[i];
		const bool wasInitailized = Player.m_bInitialized;
		Player.m_bUpdated = false;
		Player.m_bInitialized = false;

		if (!pEntity)
			continue;

		if (pEntity->GetTeam() == pLocal->GetTeam())
			continue;

		if (!pEntity->IsPlayer())
			continue;

		Player.m_bInitialized = true;
		if (!wasInitailized) { // player changed
			Player.m_flSimulationTime = -1; // force an update 
			Player.m_nBruteforce = 0; // reset bruteforce data
			Player.m_bShouldBaim = false;
			Player.m_bLbyTimingAccurate = true;
		}

		if (pEntity->IsDormant())
			continue;

		if (!pEntity->IsAlive())
			continue;

		Player.m_bUpdated = true;

		if (Player.Cache(pEntity))
			Player.m_LagRecord.emplace(Player.m_LagRecord.begin(), Record(pEntity, Player));

		if (Player.m_LagRecord.size() > 300) //128 is max amount of ticks you can backtrack (assuming highest tickrate is 128)
			Player.m_LagRecord.pop_back();
	}
}

void Accuracy::AddShot(C_CSPlayer* pLocal, C_CSPlayer *pEntity) {
	if (!pLocal) return;
	if (!pEntity) return;

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon) return;
	if (!g::aimbotting) return;
	if (!pLocal->bCanFire()) return;
	if (!(g::cmd->buttons & IN_ATTACK)) return;
	if (pLocal->IsInReload()) return;
	CBoneAccessor* boneaccessor = pEntity->GetBoneAccessor();
	if (!boneaccessor) {
		std::cout << XorStr("boneaccessor invalid") << std::endl;
		return;
	}

	const int i = pEntity->EntIndex();
	m_LastShot.m_bLbyUpdate = Players[i].m_bBacktrackedToLby;
	m_LastShot.m_nTargetIndex = i;
	m_LastShot.m_EyePos = pLocal->GetEyePos();
	m_LastShot.m_nReadableBones = boneaccessor->m_ReadableBones;
	m_LastShot.m_nWritableBones = boneaccessor->m_WritableBones;
	//if (m_LastShot.m_BoneMatrix != nullptr) {
	//	MEMALLOC->Free(m_LastShot.m_BoneMatrix);
	//	m_LastShot.m_BoneMatrix = static_cast<matrix3x4_t*>(MEMALLOC->Alloc(sizeof(boneaccessor->m_pBones)));
	//}
	//else {
	//	std::cout << XorStr("bonematrix needed new alloc") << std::endl;
	//	m_LastShot.m_BoneMatrix = static_cast<matrix3x4_t*>(MEMALLOC->Alloc(sizeof(boneaccessor->m_pBones)));
	//}
	memcpy(m_LastShot.m_BoneMatrix, boneaccessor->m_pBones, MAXSTUDIOBONES * sizeof(matrix3x4_t));
	return;
	if (VisualVars::get().ESP.g_bBulletTracer)
	{
		studiohdr_t* hdr = MODELINFO->GetStudioModel(pEntity->GetModel());

		if (!hdr)
			return;

		mstudiohitboxset_t* set = hdr->GetHitboxSet(pEntity->GetPlayerHitboxSet());

		if (!set)
			return;

		for (int i = 0; i < set->numhitboxes; i++)
		{
			mstudiobbox_t* hitbox = set->pHitbox(i);

			if (!hitbox)
				continue;

			Color col = Color::Red();

			if (hitbox->radius == -1.0f)
			{
				Vector position;
				QAngle angles;
				Math::MatrixAngles(boneaccessor->m_pBones[hitbox->bone], angles, position);
				DEBUGOVERLAY->AddBoxOverlay(position, hitbox->bbmin, hitbox->bbmax, angles, col.GetR(), col.GetG(), col.GetB(), 255, 2.5f);
			}
			else
			{
				Vector vMin, vMax;
				Math::VectorTransformZ(hitbox->bbmin, boneaccessor->m_pBones[hitbox->bone], vMin);
				Math::VectorTransformZ(hitbox->bbmax, boneaccessor->m_pBones[hitbox->bone], vMax);
				DEBUGOVERLAY->DrawPill(vMin, vMax, hitbox->radius, col.GetR(), col.GetG(), col.GetB(), 255, 2.5f);
			}
		}
	}
}

void Accuracy::ShotReceived(Vector impact) {
	m_bShotReceived = true;
	m_Impact = impact;
}

void Accuracy::PlayerHurt(C_CSPlayer *pEntity) {
	if (!pEntity) return;
	m_nHurtIndex = pEntity->EntIndex();
}

void Accuracy::BacktrackPlayer(C_CSPlayer *pEntity, BacktrackedPlayer &Player) {
	if (!pEntity) return;

	CBoneAccessor* boneaccessor = pEntity->GetBoneAccessor();

	Record &restore = Player.m_LagRecord[Player.m_nSelectedTick]; //changed to const reference so we arent copying the entire record every time
	if (boneaccessor == nullptr || !restore.m_bMatrixBuilt) {
		std::cout << XorStr("bone invalid") << std::endl;
		return;
	}

	//if (boneaccessor != nullptr) {
	//	MEMALLOC->Free(boneaccessor->m_pBones);
	//	boneaccessor->m_pBones = static_cast<matrix3x4_t*>(MEMALLOC->Alloc(sizeof(restore.m_BoneMatrix)));
	//}
	//else {
	//	std::cout << XorStr("bones needed new alloc") << std::endl;
	//	boneaccessor->m_pBones = static_cast<matrix3x4_t*>(MEMALLOC->Alloc(sizeof(restore.m_BoneMatrix)));
	//}


	boneaccessor->m_ReadableBones = boneaccessor->m_WritableBones = MAXSTUDIOBONES;
	memcpy(boneaccessor->m_pBones, restore.m_BoneMatrix, MAXSTUDIOBONES * sizeof(matrix3x4_t));

	pEntity->GetSimulationTime() = restore.m_flSimulationTime;
	Player.m_bBacktracked = true;
	Player.m_bBacktrackedToLby = restore.m_nLbyUpdated;
}

void Accuracy::BacktrackPlayerToTick(C_CSPlayer *pEntity, BacktrackedPlayer &Player, Record &restore) {
	if (!pEntity) return;

	CBoneAccessor* boneaccessor = pEntity->GetBoneAccessor();

	if (boneaccessor == nullptr || !restore.m_bMatrixBuilt) {
		std::cout << XorStr("bone invalid") << std::endl;
		return;
	}

	//if (boneaccessor != nullptr) {
	//	MEMALLOC->Free(boneaccessor->m_pBones);
	//	boneaccessor->m_pBones = static_cast<matrix3x4_t*>(MEMALLOC->Alloc(sizeof(restore.m_BoneMatrix)));
	//}
	//else {
	//	std::cout << XorStr("bones needed new alloc") << std::endl;
	//	boneaccessor->m_pBones = static_cast<matrix3x4_t*>(MEMALLOC->Alloc(sizeof(restore.m_BoneMatrix)));
	//}

	boneaccessor->m_ReadableBones = boneaccessor->m_WritableBones = MAXSTUDIOBONES;
	memcpy(boneaccessor->m_pBones, restore.m_BoneMatrix, MAXSTUDIOBONES * sizeof(matrix3x4_t));

	pEntity->GetSimulationTime() = restore.m_flSimulationTime;
	Player.m_bBacktracked = true;
	Player.m_bBacktrackedToLby = restore.m_nLbyUpdated;
}

void Accuracy::BacktrackLegit(C_CSPlayer* pLocal, C_CSPlayer* pEntity)
{
	if (!pLocal)
		return;

	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].resolver.g_bBackTracking)
		return;

	if (!pEntity)
		return;

	if (pEntity->IsDormant())
		return;

	if (!pEntity->IsAlive())
		return;

	if (pEntity->GetTeam() == pLocal->GetTeam())
		return;

	if (!pEntity->IsPlayer())
		return;

	CBoneAccessor* boneaccessor = pEntity->GetBoneAccessor();

	if (!boneaccessor) {
		std::cout << XorStr("Boneaccesor invalid") << std::endl;
		return;
	}

	BacktrackedPlayer& Player = Players[pEntity->EntIndex()];

	Player.m_bBacktracked = false;

	if (SelectTickLegit(pLocal, pEntity, Player))
		BacktrackPlayer(pEntity, Player);
}

void Accuracy::Backtrack() {
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].resolver.g_bBackTracking)
		return;

	for (int i = 1; i <= 64; i++)
	{
		C_CSPlayer* pEntity = C_CSPlayer::GetPlayerByIndex(i);

		if (!pEntity)
			continue;

		if (pEntity->IsDormant())
			continue;

		if (!pEntity->IsAlive())
			continue;

		if (pEntity->GetTeam() == pLocal->GetTeam())
			continue;

		if (!pEntity->IsPlayer())
			continue;

		BacktrackedPlayer &Player = Players[i];

		Player.m_bBacktracked = false;

		if (!Player.m_bUpdated || !Player.m_bInitialized)
			continue;

		if (SelectTick(pEntity, Player))
			BacktrackPlayer(pEntity, Player);
		else if(MenuVars::get().g_bResolveYaw)
			Resolve(pEntity, Player);
		
	}
}

bool Accuracy::SelectTickLegit(C_CSPlayer* pLocal, C_CSPlayer* pEntity, BacktrackedPlayer &Player) {
	if (!pLocal) return false;
	if (!pEntity) return false;

	Vector localpos = pLocal->GetEyePos();

	bool ret = false;
	float bestfov = 9999.f;
	for (auto i = 0; i < Player.m_LagRecord.size(); i++)
	{
		if (!is_tick_valid(Player.m_LagRecord[i].m_flSimulationTime))
			continue;

		Vector aimspotpos = Player.m_LagRecord[i].m_vecHeadSpot;

		static auto is_visible = [](Vector& src, Vector& dst, C_CSPlayer* pLocal, C_CSPlayer* pEntity) -> bool {
			Ray_t ray(src, dst);
			CTraceFilter filter(pLocal);
			trace_t tr;
			ENGINETRACE->TraceRay(ray, MASK_SHOT, &filter, &tr);

			return tr.m_pEnt == pEntity || (tr.fraction > .97f && tr.m_pEnt != ENTITYLIST->GetClientEntity(0));
		};

		if (!is_visible(localpos, aimspotpos, pLocal, pEntity))
			continue;

		const float fov = Math::GetFov(Math::CalcAngle(localpos, aimspotpos), g::cmd->viewangles);

		if (abs(fov) < bestfov)
		{
			bestfov = fov;
			Player.m_nSelectedTick = i;
			ret = true;
		}
	}

	return ret;
}

bool Accuracy::SelectTick(C_CSPlayer *pEntity, BacktrackedPlayer &Player) {
	if (!pEntity) return false;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return false;

	bool ret = false;
	float lowestThickness = 0;
	Vector local_eyes = pLocal->GetEyePos();

	for (auto i = 0; i < Player.m_LagRecord.size(); i++) {
		if (!is_tick_valid(Player.m_LagRecord[i].m_flSimulationTime))
			continue;
		Record &restore = Player.m_LagRecord[i];
		if (restore.m_nLbyUpdated || !MenuVars::get().g_bResolveYaw) {
			float thickness = GetThickness(local_eyes, restore.m_vecHeadSpot);
			if (thickness < lowestThickness || !ret) {
				Player.m_nSelectedTick = i;
				lowestThickness = thickness;
			}
			ret = true;
		}
	}

	//if(ret)
	//	std::cout << Player.m_nSelectedTick << std::endl;

	return ret;
}

void Accuracy::RestorePlayer(C_CSPlayer *pEntity, BacktrackedPlayer &Player) {
	if (!pEntity) return;
	pEntity->GetSimulationTime() = Player.m_flSimulationTime;
	//pEntity->GetFlags() = Player.m_nFlags;
	pEntity->SetAbsAngles(Player.m_angAbsAngles);
	pEntity->SetAbsOrigin(Player.m_vecAbsOrgin);
	pEntity->GetEyeAngles() = Player.m_angAngles;
	pEntity->GetOrigin() = Player.m_vecOrigin;
	//pEntity->GetCollideable()->OBBMaxs() = Player.m_vecMaxs;
	//pEntity->GetCollideable()->OBBMins() = Player.m_vecMins;
	//pEntity->GetPoseParameters() = Player.m_arrflPoseParameters;
	//
	//pEntity->GetSequence() = Player.m_nSequence;
	//pEntity->GetCycle() = Player.m_flCycle;
	//const int layerCount = pEntity->GetNumAnimOverlays();
	//for (int i = 0; i < layerCount; i++) {
	//	C_AnimationLayer &currentLayer = pEntity->GetAnimOverlay2(i);
	//	currentLayer.m_nOrder = Player.m_LayerRecords[i].m_nOrder;
	//	currentLayer.m_nSequence = Player.m_LayerRecords[i].m_nSequence;
	//	currentLayer.m_flWeight = Player.m_LayerRecords[i].m_flWeight;
	//	currentLayer.m_flCycle = Player.m_LayerRecords[i].m_flCycle;
	//}
	//auto old_curtime = GLOBALS->curtime;
	//auto old_frametime = GLOBALS->frametime;
	//
	//GLOBALS->curtime = pEntity->GetSimulationTime();
	//GLOBALS->frametime = GLOBALS->interval_per_tick;
	//C_CSGOPlayerAnimState *anim_state = g::plist[pEntity->EntIndex()].Animations.GetAnimState( pEntity );
	//C_CSGOPlayerAnimState *anim_state = pEntity->GetPlayerAnimState();
	//if (anim_state) {
	//	anim_state->m_flLastClientSideAnimationUpdateTime = 0;
	//	anim_state->m_iLastClientSideAnimationUpdateFramecount = 0;
	//	anim_state->UpdateAnimationState(pEntity->GetEyeAngles());
	//	pEntity->UpdateClientSideAnimation();
	//}
	//GLOBALS->curtime = old_curtime;
	//GLOBALS->frametime = old_frametime;
}

void Accuracy::RestorePlayers() {
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();
	if (!pLocal)
		return;

	for (int i = 1; i <= 64; i++) {
		C_CSPlayer* pEnt = C_CSPlayer::GetPlayerByIndex(i);
		if (!pEnt) continue;
		if (pEnt->IsDormant()) continue;
		if (!pEnt->IsAlive()) continue;
		if (pEnt->GetTeam() == pLocal->GetTeam()) continue;
		if (!pEnt->IsPlayer()) continue;

		BacktrackedPlayer &Player = Players[i];

		if (!Player.m_bUpdated || !Player.m_bInitialized)
			continue;

		RestorePlayer(pEnt, Player);

		pEnt->InvalidateBoneCache();

		CBoneAccessor* boneaccessor = pEnt->GetBoneAccessor();

		if (!boneaccessor) {
			std::cout << XorStr("Boneaccesor invalid") << std::endl;
			continue;
		}

		boneaccessor->m_ReadableBones = boneaccessor->m_WritableBones = MAXSTUDIOBONES;
		pEnt->SetupBones(boneaccessor->m_pBones, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0);
	}
}

//entry point
void Accuracy::FrameStageNotify(ClientFrameStage_t stage) {
	if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		StorePlayers();
}
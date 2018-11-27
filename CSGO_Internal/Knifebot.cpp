#include "Knifebot.h"
#include "Globals.h"


bool Knifebot::CanHit(bool& frombehind, bool stab)
{
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return false;

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return false;

	if (!weapon->IsKnife())
		return false;

	/*if (!pLocal->bCanFire())
	return false;*/

	Vector Forward;
	Math::AngleVectors(g::cmd->viewangles, &Forward);

	Vector ForwardBackup(Forward);

	Forward *= stab ? 32.f : 48.f; //calc distance based on right or left swing

	Vector Start = pLocal->GetEyePos();
	Vector End = Start + Forward;

	trace_t tr;
	Utils::TraceLine(Start, End, MASK_SHOT, pLocal, &tr);

	CTraceFilter filter;
	filter.pSkip = pLocal;

	if (tr.fraction == 1.0f)
		Utils::TraceHull(Start, End, HeadMins, HeadMaxs, &filter, &tr);

	if (tr.fraction < 1.0f)
	{
		C_CSPlayer* ent = (C_CSPlayer*)tr.m_pEnt;

		if (ent->IsDormant())
			return false;

		if (!ent->IsAlive())
			return false;

		if (ent->GetGunGameImmunity())
			return false;

		if (ent->GetTeam() == pLocal->GetTeam())
			return false;

		ClientClass* client_class = ent->GetClientClass();

		if (!client_class)
			return false;

		if (client_class->m_ClassID != CCSPlayer)
			return false;

		Math::AngleVectors(ent->GetEyeAngles(), &Forward);

		Vector MyLOS(ForwardBackup.x, ForwardBackup.y, 0);
		Math::GUWOPNormalize(MyLOS);

		Vector TargetLOS(Forward.x, Forward.y, 0);
		Math::GUWOPNormalize(TargetLOS);

		float DotProduct = MyLOS.DotProduct(MyLOS, TargetLOS);

		if (DotProduct > .8f)
			frombehind = true;

		return true;
	}

	return false;
}

void Knifebot::CreateMove()
{
	globals::get().knifebotting = false;
	if (!MiscVars::get().g_bAutoKnife)
		return;

	bool backstab;
	//second parameter determines if we allow stabs or not
	bool canhit = CanHit(backstab, true);


	if (canhit) 
	{
		//G::UserCmd->buttons |= backstab ? IN_ATTACK2 : IN_ATTACK;
		g::cmd->buttons |= IN_ATTACK2;
		globals::get().knifebotting = true;
	}
}

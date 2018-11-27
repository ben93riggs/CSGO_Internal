#include "ClosestToCrosshair.h"
#include "Globals.h"

bool closest_to_crosshair::is_valid_target(C_CSPlayer* ent, C_CSPlayer* p_local)
{
	if (!p_local)
		return false;
	if (!ent)
		return false;
	if (ent == p_local)
		return false;
	ClientClass* ent_client_class = ent->GetClientClass();
	if (!ent_client_class) return false;
	if (ent_client_class->m_ClassID != CCSPlayer)
		return false;
	if (g::plist[ent->EntIndex()].corrections.bWhiteListed)
		return false;
	if (!ent->IsAlive())
		return false;
	if (ent->IsDormant())
		return false;
	if (ent->GetTeam() == p_local->GetTeam())
		return false;
	if (ent->GetTeam() != 2 && ent->GetTeam() != 3)
		return false;
	if (ent->GetGunGameImmunity())
		return false;

	return true;
}

void closest_to_crosshair::run(CUserCmd* cmd)
{
	const auto p_local = C_CSPlayer::GetLocalPlayer();

	globals::get().closest_to_crosshair = nullptr;

	auto best_fov = 999.f;
	for (auto i = 0; i < 64; i++)
	{
		if (i == ENGINE->GetLocalPlayer())
			continue;

		const auto p_entity = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));

		if (!is_valid_target(p_entity, p_local))
			continue;

		const auto fov = Math::GetFov(QAngle(0, Math::CalcAngle(p_local->GetEyePos(), p_entity->GetEyePos()).y, 0), cmd->viewangles);
		if (fov < best_fov)
		{
			globals::get().closest_to_crosshair = p_entity;
			best_fov = fov;
		}
	}
}

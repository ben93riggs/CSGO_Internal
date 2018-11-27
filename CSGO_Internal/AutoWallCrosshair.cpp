#include "AutoWallCrosshair.h"
#include "Globals.h"
#include "Autowall.h"
#include "CRender.h"

void AutoWallCrosshair::PaintTraverse(float centerx, float centery) const
{
	if (MenuVars::get().g_iCurrentAimbotMode < 1)
		return;

	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_bAutoWall)
		return;

	if (!ENGINE->IsConnected() || !ENGINE->IsInGame())
		return;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (!pLocal->IsAlive())
		return;

	CBaseCombatWeapon* local_weapon = pLocal->GetActiveWeapon();

	if (!local_weapon)
		return;

	auto weapon_data = local_weapon->GetCSWpnData();

	if (!weapon_data)
		return;

	Vector EyeAng;
	ENGINE->GetViewAngles(EyeAng);

	Vector forward;
	Math::AngleVectors(EyeAng, &forward);

	Vector dst = pLocal->GetEyePos() + (forward * weapon_data->flRange);

	float damage = CAutoWall::get().can_hit(dst, pLocal);

	CRender::get().DrawText(centerx, centery - 20, Color::White(), true, XorStr("~%.f dmg"), damage);
}

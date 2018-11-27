#include "RecoilCrosshair.h"
#include "Globals.h"
#include "Config.h"

#include "CRender.h"

void RecoilCrosshair::PaintTraverse(C_CSPlayer * pLocal, int iScreenHeight, int iScreenWidth, int centerx, int centery) const
{
	if (!pLocal)
		return;

	if (MiscVars::get().Crosshair.g_iStyle)
	{
		Color color = Color(MiscVars::get().Crosshair.g_cColor[0] * 255, MiscVars::get().Crosshair.g_cColor[1] * 255, MiscVars::get().Crosshair.g_cColor[2] * 255, MiscVars::get().Crosshair.g_cColor[3] * 255);

		int gap = MiscVars::get().Crosshair.g_iGap;

		if (MiscVars::get().Crosshair.g_bDynamic)
		{
			if (!pLocal)
				return;

			CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

			if (!weapon)
				return;

			gap = weapon->GetInaccuracy() / weapon->GetSpread() + 1;
			gap *= .25;
			if (gap > 50)
				gap = 50;
		}

		if (MiscVars::get().Crosshair.g_bDot)
		{
			for (int i = 0; i <= MiscVars::get().Crosshair.g_iThickness - 1; i++)
			{
				CRender::get().DrawCircle(centerx, centery, 1 + i / 2, 64, color);
			}
		}

		switch (MiscVars::get().Crosshair.g_iStyle)
		{
		case 1: //normal
			CRender::get().DrawFilledRect(centerx - (MiscVars::get().Crosshair.g_iLength + gap), centery - MiscVars::get().Crosshair.g_iThickness / 2, MiscVars::get().Crosshair.g_iLength, MiscVars::get().Crosshair.g_iThickness, color);
			CRender::get().DrawFilledRect(centerx + gap, centery - MiscVars::get().Crosshair.g_iThickness / 2, MiscVars::get().Crosshair.g_iLength, MiscVars::get().Crosshair.g_iThickness, color);
			CRender::get().DrawFilledRect(centerx - MiscVars::get().Crosshair.g_iThickness / 2, centery - (MiscVars::get().Crosshair.g_iLength + gap), MiscVars::get().Crosshair.g_iThickness, MiscVars::get().Crosshair.g_iLength, color);
			CRender::get().DrawFilledRect(centerx - MiscVars::get().Crosshair.g_iThickness / 2, centery + gap, MiscVars::get().Crosshair.g_iThickness, MiscVars::get().Crosshair.g_iLength, color);
			break;
		case 2: //circle
			for (int i = 0; i <= MiscVars::get().Crosshair.g_iThickness - 1; i++)
			{
				CRender::get().DrawCircle(centerx, centery, gap + i, 64, color);
			}
			break;
		default:
			break;
		}
	}

	if (!pLocal->IsAlive())
		return;

	CBaseCombatWeapon* local_weapon = pLocal->GetActiveWeapon();

	if (!local_weapon)
		return;

	if (VisualVars::get().Removals.g_bNoScope && local_weapon && local_weapon->GetZoomLevel() && pLocal->bCanFire() && local_weapon->GetWeaponType() == WEAPONTYPE_SNIPER)
	{
		CRender::get().DrawLine(0, iScreenHeight / 2, iScreenWidth, iScreenHeight / 2, Color::Black());
		CRender::get().DrawLine(iScreenWidth / 2, 0, iScreenWidth / 2, iScreenHeight, Color::Black());
	}

	if (!VisualVars::get().g_bDrawSpread)
		return;

	Vector screen_center(iScreenWidth * 0.5f, iScreenHeight * 0.5f, 0);
	int x2 = screen_center.x;
	int y2 = screen_center.y;

	if (g::current_fov == 0)
		return;

	float dy = iScreenHeight / g::current_fov;
	float dx = iScreenWidth / g::current_fov;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bRCSEnabled && !WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bSilentRCS)
	{
		auto punch = pLocal->AimPunch();
		if (punch.x != 0.0f || punch.y != 0.0f)
		{
			x2 -= dx * punch.y;
			y2 += dy * punch.x;
		}
	}

	float cross_x, cross_y = 0.f;

	if (WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bStandaloneRCS && WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bSilentRCS)
	{
		cross_x = screen_center.x;
		cross_y = screen_center.y;
	}
	else if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bStandaloneRCS && WeaponConfigVars::get().WeaponConfig[g::weaponconfig].legitbot.g_bSilentRCS)
	{
		/*cross_x = (x - (pLocal->AimPunch().y * 2.f * dx));*/
		cross_x = (screen_center.x - (pLocal->AimPunch().y * dx));
	}
	else
	{
		cross_x = screen_center.x - pLocal->AimPunch().y * dx;
		cross_y = screen_center.y + pLocal->AimPunch().x * dy;
	}

	if (cross_x <= 0 || cross_y <= 0)
		return;

	float spread_dist = (local_weapon->GetInaccuracy() + local_weapon->GetSpread()) * 320.f / std::tan(DEG2RAD(g::current_fov) / 2.f);
	float spread_radius = spread_dist * (iScreenHeight / 480.f);

	CRender::get().DrawFilledCircle(Vector2D(cross_x, cross_y), 64, spread_radius, Color::Black(100));
}

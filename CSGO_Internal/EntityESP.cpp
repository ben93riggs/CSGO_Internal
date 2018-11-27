#include "EntityESP.hpp"
#include "Utils.hpp"
#include "Config.h"
#include "Globals.h"

#include "BombDamage.h"
#include "CRender.h"
#include "Accuracy.h"

#define WEAPON_IS_ONTARGET				0x40
#define WEAPON_NOT_CARRIED				0	// Weapon is on the ground
#define WEAPON_IS_CARRIED_BY_PLAYER		1	// This client is carrying this weapon.
#define WEAPON_IS_ACTIVE				2	// This client is carrying this weapon and it's the currently held weapon

void entity_esp::do_player_esp(C_CSPlayer* local, C_CSPlayer* entity) const
{
	if (!local)
		return;

	if (!entity)
		return;

	auto offsety = 0;

	Color color;

	if (VisualVars::get().ESP.g_bVisCheck)
	{
		if (!C_CSPlayer::IsVisible(Utils::GetHitboxPosition(entity, HITBOX_HEAD)) &&
			!C_CSPlayer::IsVisible(Utils::GetHitboxPosition(entity, HITBOX_LEFT_FOOT)) &&
			!C_CSPlayer::IsVisible(Utils::GetHitboxPosition(entity, HITBOX_RIGHT_FOOT)))
			return;
	}

#pragma region bounding boxes setup

	const auto trans = entity->GetCoordinateFrame();

	const auto ent_client_renderable = entity->GetCollideable();

	if (!ent_client_renderable)
		return;

	const auto min = ent_client_renderable->OBBMaxs();
	const auto max = ent_client_renderable->OBBMins();

	Vector points[] =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector points_transformed[8];

	for (int i = 0; i < 8; i++)
	{
		Math::VectorTransform(points[i], trans, points_transformed[i]);
	}

	Vector flb;
	Vector brt;
	Vector blb;
	Vector frt;
	Vector frb;
	Vector brb;
	Vector blt;
	Vector flt;

	if (!Math::WorldToScreen(points_transformed[3], flb) || !Math::WorldToScreen(points_transformed[5], brt)
		|| !Math::WorldToScreen(points_transformed[0], blb) || !Math::WorldToScreen(points_transformed[4], frt)
		|| !Math::WorldToScreen(points_transformed[2], frb) || !Math::WorldToScreen(points_transformed[1], brb)
		|| !Math::WorldToScreen(points_transformed[6], blt) || !Math::WorldToScreen(points_transformed[7], flt))
		return;

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;        // left
	float top = flb.y;        // top
	float right = flb.x;    // right
	float bottom = flb.y;    // bottom

	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
	}

	const Vector top_center(left + ((right - left) * 0.5f), top, 0);
	const Vector bottom_center(left + ((right - left) * 0.5f), bottom, 0);
	const int height = abs(top - bottom);
	const int width = abs(right - left);

#pragma endregion

	matrix3x4_t matrix[MAXSTUDIOBONES];
	if (!entity->SetupBones(matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, entity->GetSimulationTime()))
		return;

	//get color based on team
	if (local->GetTeam() == entity->GetTeam())
	{
		if (entity->IsDormant())
			return;

		if (VisualVars::get().ESP.g_bShowTeammates)
		{
			color = Color(VisualVars::get().ESP.g_cESPAllyColor[0] * 255.f, VisualVars::get().ESP.g_cESPAllyColor[1] * 255.f, VisualVars::get().ESP.g_cESPAllyColor[2] * 255.f, entity->IsDormant() ? 30.f : 255.f);
		}
		else return;
	}
	else
	{
		color = Color(VisualVars::get().ESP.g_cESPEnemyColor[0] * 255.f, VisualVars::get().ESP.g_cESPEnemyColor[1] * 255.f, VisualVars::get().ESP.g_cESPEnemyColor[2] * 255.f);
	}

	if (entity->IsDormant())
		color = Color::DarkGray(30);

	if (VisualVars::get().ESP.g_bShowSnapLines)
		CRender::DrawLine(bottom_center.x, bottom_center.y, g::screenx / 2, g::screeny, color);

	draw_backtrack_skeletons(local, entity, color);

	if (VisualVars::get().ESP.g_iShowHitboxes == 2)
	{
		draw_hitboxes(entity, color, matrix);
		/*studiohdr_t* hdr = MODELINFO->GetStudioModel(entity->GetModel());

		if (!hdr)
		return;

		mstudiohitboxset_t* set = hdr->GetHitboxSet(entity->GetPlayerHitboxSet());

		if (!set)
		return;

		for (int i = 0; i < set->numhitboxes; i++)
		{
		mstudiobbox_t* hitbox = set->pHitbox(i);

		if (!hitbox)
		continue;

		if (hitbox->radius == -1.0f)
		{
		Vector position;
		QAngle angles;
		Math::MatrixAngles(matrix[hitbox->bone], angles, position);
		DEBUGOVERLAY->AddBoxOverlay(position, hitbox->bbmin, hitbox->bbmax, angles, color.GetR(), color.GetG(), color.GetB(), 255, GLOBALS->frametime * 2.f);
		}
		else
		{
		Vector vMin, vMax;
		Math::VectorTransformZ(hitbox->bbmin, matrix[hitbox->bone], vMin);
		Math::VectorTransformZ(hitbox->bbmax, matrix[hitbox->bone], vMax);
		DEBUGOVERLAY->DrawPill(vMin, vMax, hitbox->radius, color.GetR(), color.GetG(), color.GetB(), 255, GLOBALS->frametime * 2.f);
		}
		}*/
	}
	else if (VisualVars::get().ESP.g_iShowHitboxes == 1)
	{
		draw_head(entity, color, matrix);
	}

	if (VisualVars::get().ESP.g_bESPOverlay)
	{
		CRender::DrawFilledRect(left, top, right - left, bottom - top, entity->IsDormant() ? Color::DarkGray(30) : Color(color.GetR(), color.GetG(), color.GetB(), 20.f));
	}

	if (VisualVars::get().ESP.g_iESPBox)
	{
		switch (VisualVars::get().ESP.g_iESPBox)
		{
		case BOX_TWO_DIMENSIONAL:
			CRender::DrawLineWithOutline(Vector(left, bottom, 0), Vector(left, top, 0), color);
			CRender::DrawLineWithOutline(Vector(left, bottom, 0), Vector(right, bottom, 0), color);
			CRender::DrawLineWithOutline(Vector(left, top, 0), Vector(right, top, 0), color);
			CRender::DrawLineWithOutline(Vector(right, bottom, 0), Vector(right, top, 0), color);
			break;
		case BOX_CORNERS:
			CRender::DrawCornerESP(left, right, top, bottom, color);
			break;
		case BOX_THREE_DIMENSIONAL:
			CRender::DrawLineWithOutline(blb, brb, color);
			CRender::DrawLineWithOutline(blb, blt, color);
			CRender::DrawLineWithOutline(blb, flb, color);
			CRender::DrawLineWithOutline(flb, frb, color);
			CRender::DrawLineWithOutline(flb, flt, color);
			CRender::DrawLineWithOutline(brb, brt, color);
			CRender::DrawLineWithOutline(brb, frb, color);
			CRender::DrawLineWithOutline(frb, frt, color);
			CRender::DrawLineWithOutline(blt, brt, color);
			CRender::DrawLineWithOutline(blt, flt, color);
			CRender::DrawLineWithOutline(brt, frt, color);
			CRender::DrawLineWithOutline(flt, frt, color);
			break;
		default: break;
		}
	}

	if (VisualVars::get().ESP.g_bShowHealthbars)
	{
		const auto i_health = entity->GetHealth();
		const int rr = 255 - (i_health * 2.55);
		const int gg = i_health * 2.55f;

		//scale based on distance
		CRender::DrawFilledRect(left - 6, top, 4, height, Color(1, 1, 1, entity->IsDormant() ? 10 : 90)); //black outline
		CRender::DrawFilledRect(left - 5, top + 1, 2, height - 2, Color(rr, gg, 0, entity->IsDormant() ? 30 : 255)); //health
		CRender::DrawFilledRect(left - 5, top + 1, 2, (height - (height * (static_cast<float>(i_health) / 100.0f))) - 2, Color(1, 1, 1, entity->IsDormant() ? 30 : 255)); //black fill (missing health)
		CRender::get().DrawText(left - 15, top + height - 5 - (height * (static_cast<float>(i_health) / 100.0f)), entity->IsDormant() ? Color::DarkGray(30) : Color::White(), true, "%d", i_health);

		//draw lines to make each cell
		//for (int i = 1; i <= 9; i++)
		//	CRender::get().DrawLine(left - 6 - offsetx, top - 1 + (height / 10 * i), left - 6 - offsetx + 5, top - 1 + (height / 10 * i), Color::Black());
	}

	if (MenuVars::get().g_bResolveYaw)
	{
		if (Accuracy::get().Players[entity->EntIndex()].m_bLbyTimingAccurate)
		{
			const auto curtime = entity->GetSimulationTime();
			const auto updatetime = Accuracy::get().Players[entity->EntIndex()].m_flLbyUpdate + 1.1f;
			const auto deltatime = updatetime - curtime;
			auto percent = 1.f - min(deltatime / 1.1f, 1.f);

			percent = Math::clamp<float>(percent, 0.f, 1.f);

			CRender::DrawFilledRect(left, bottom + 3 + offsety, width, 4, Color(1, 1, 1, entity->IsDormant() ? 30 : 255));
			CRender::DrawFilledRect(left + 1, bottom + 4 + offsety, (width * percent) - 2, 2, Color::LightSalmon(entity->IsDormant() ? 30 : 255));

			offsety += 12;
		}
	}

	if (VisualVars::get().ESP.g_bShowArmorbars)
	{
		const auto i_armor = static_cast<float>(entity->GetArmorValue());

		CRender::DrawFilledRect(left, bottom + 3 + offsety, width, 4, Color(1, 1, 1, entity->IsDormant() ? 30 : 255)); //black box
		CRender::DrawFilledRect(left + 1, bottom + 4 + offsety, (width * (i_armor / 100.f)) - 2, 2, Color(0, 127, 255, entity->IsDormant() ? 30 : 255)); //armor bar
		CRender::get().DrawText(left + 1 + (width * (i_armor / 100.f)), bottom + offsety, entity->IsDormant() ? Color::DarkGray(30) : Color::White(), true, "%d", static_cast<int>(i_armor)); //text

		offsety += 12;
	}

	if (VisualVars::get().ESP.g_bShowSkeletons)
	{
		const auto p_studio_model = MODELINFO->GetStudioModel(entity->GetModel());

		if (!p_studio_model)
			return;

		for (auto i = 0; i < p_studio_model->numbones; i++)
		{
			const auto p_bone = p_studio_model->pBone(i);
			if (!p_bone || !(p_bone->flags & BONE_USED_BY_HITBOX) || p_bone->parent == -1)
				continue;

			Vector v_bone_pos1;
			if (!Math::WorldToScreen(Vector(matrix[i][0][3], matrix[i][1][3], matrix[i][2][3]), v_bone_pos1))
				continue;

			Vector v_bone_pos2;
			if (!Math::WorldToScreen(Vector(matrix[p_bone->parent][0][3], matrix[p_bone->parent][1][3], matrix[p_bone->parent][2][3]), v_bone_pos2))
				continue;

			CRender::DrawLine(v_bone_pos1.x, v_bone_pos1.y, v_bone_pos2.x, v_bone_pos2.y, color);
		}

	}

	if (VisualVars::get().ESP.g_bShowGunBarrelLines)
	{
		const auto vec_eye = entity->GetEyePos();
		Vector vec_end;
		Vector vec2_d_end;
		Vector vec2_d_start;

		trace_t tr;
		CTraceFilter filter;
		filter.pSkip = entity;

		Ray_t ray;

		Math::AngleVectors(entity->GetEyeAngles(), &vec_end);

		auto weapon = entity->GetActiveWeapon();

		auto weaponrange = 8192.0f;

		if (weapon)
		{
			const auto weapondata = weapon->GetCSWpnData();

			if (weapondata)
			{
				weaponrange = weapondata->flRange;
			}
		}

		vec_end = vec_end * weaponrange + vec_eye;

		ray.Init(vec_eye, vec_end);


		ENGINETRACE->TraceRay(ray, MASK_SHOT, &filter, &tr);

		const auto muzzle = tr.endpos;

		if (Math::WorldToScreen(muzzle, vec2_d_end) && Math::WorldToScreen(vec_eye, vec2_d_start))
		{
			CRender::DrawLine(vec2_d_start.x, vec2_d_start.y, vec2_d_end.x, vec2_d_end.y, color);
			CRender::DrawCircle(vec2_d_end.x, vec2_d_end.y, 3, 3 * M_PI, color);
		}
	}

	if (VisualVars::get().ESP.g_bShowBomb)
	{
		if (entity->EntIndex() == PLAYERRESOURCE->GetPlayerC4())
		{
			if (VisualVars::get().ESP.g_bShowBomb == ICON_TEXT)
				CRender::get().DrawText(bottom_center.x, bottom_center.y + offsety, entity->IsDormant() ? Color::GreenYellow(30) : Color::GreenYellow(), true, XorStr("C4"));
			else if (VisualVars::get().ESP.g_bShowWeaponNames == ICON_ON)
				CRender::get().DrawIcon(bottom_center.x, bottom_center.y + offsety, entity->IsDormant() ? Color::DarkGray(30) : Color::GreenYellow(), true, "%s", std::string(1, '\\').c_str());
		}
	}

	if (VisualVars::get().ESP.g_bShowNames)
	{
		player_info_t p_info;
		ENGINE->GetPlayerInfo(entity->EntIndex(), &p_info);

		CRender::get().DrawText(top_center.x, top_center.y - 14, entity->IsDormant() ? Color::DarkGray(30) : Color::White(), true, "%s", p_info.m_szPlayerName);
	}

	if (VisualVars::get().ESP.g_bShowAmmo)
	{
		auto p_base_weapon = entity->GetActiveWeapon();
		if (p_base_weapon)
		{
			const auto clip1 = p_base_weapon->GetClip1();
			const auto wep_info = p_base_weapon->GetCSWpnData();
			if (wep_info)
			{
				const auto clip_size = wep_info->iMaxClip1;

				if (clip1 >= 0 && clip_size > 0)
				{
					CRender::DrawFilledRect(left, bottom + 3 + offsety, width, 4, clip1 == 0 ? Color(255, 255, 255, entity->IsDormant() ? 10 : 90) : Color(1, 1, 1, entity->IsDormant() ? 10 : 90)); //black box
					CRender::DrawFilledRect(left + 1, bottom + 4 + offsety, width - 2, 2, clip1 == 0 ? Color(255, 255, 255, entity->IsDormant() ? 30 : 255) : Color(1, 1, 1, entity->IsDormant() ? 30 : 255)); //black box
					CRender::DrawFilledRect(left + 1, bottom + 4 + offsety, (width * (static_cast<float>(clip1) / static_cast<float>(clip_size))) - 2, 2, Color(255, 200, 0, entity->IsDormant() ? 30 : 255)); //armor bar

					if (clip1 > 0)
						CRender::get().DrawText(left + (width * (static_cast<float>(clip1) / static_cast<float>(clip_size))) - 2, bottom + offsety, entity->IsDormant() ? Color::DarkGray(30) : Color::White(), true, XorStr("%d"), clip1);

					offsety += 10;
				}
			}
		}
	}

	if (VisualVars::get().ESP.g_bShowWeaponNames)
	{
		auto p_base_weapon = entity->GetActiveWeapon();

		if (!p_base_weapon)
			return;

		if (p_base_weapon->GetState() == WEAPON_IS_ACTIVE)
		{
			std::string name = p_base_weapon->GetClientClass()->m_pNetworkName;

			if (name[0] == 'C')
			{
				if (name[1] == 'W')
				{
					name = name.substr(7, name.size() - 7);
				}
				else
				{
					name = name.substr(1, name.size() - 1);
				}
			}

			if (VisualVars::get().ESP.g_bShowWeaponNames == ICON_TEXT) 
			{
				CRender::get().DrawText(bottom_center.x, bottom_center.y + offsety, entity->IsDormant() ? Color::DarkGray(30) : Color::White(), true, "%s", name.c_str());
				offsety += 12;
			}
			else if (VisualVars::get().ESP.g_bShowWeaponNames == ICON_ON) 
			{
				CRender::get().DrawIcon(bottom_center.x, bottom_center.y + offsety, entity->IsDormant() ? Color::DarkGray(30) : Color::White(), true, "%s", std::string(1, p_base_weapon->GetWeaponIcon()).c_str());
				offsety += 12;
			}

		}
	}

	if (g::plist[entity->EntIndex()].overridden)
		CRender::get().DrawText(right + 12, top, entity->IsDormant() ? Color::Red(0) : Color::Red(), false, XorStr("OVERRIDDEN"));

}

void entity_esp::draw_bomb_damage(C_CSPlayer* entity, const int local_hp)
{
	if (!entity)
		return;//Null check 

	const auto p_local = C_CSPlayer::GetLocalPlayer();

	auto color = Color(VisualVars::get().ESP.g_cESPEnemyColor[0] * 255.f, VisualVars::get().ESP.g_cESPEnemyColor[1] * 255.f, VisualVars::get().ESP.g_cESPEnemyColor[2] * 255.f);

	const auto v_origin = entity->GetAbsOrigin();

	const auto f_bomb_damage = BombDamage::GetBombDamage(p_local, entity);

	if (f_bomb_damage >= local_hp)
		color = Color(255, 0, 0, 255);

	Vector v_screen_origin;
	if (Math::WorldToScreen(v_origin, v_screen_origin)) 
	{
		CRender::DrawCircle(v_screen_origin.x, v_screen_origin.y, 8, 128, color);
		CRender::get().DrawText(v_screen_origin.x, v_screen_origin.y, color, true, XorStr("Time Remaining: %.2f | [~%d]"), entity->GetBombTimer(), f_bomb_damage);
	}
}

void entity_esp::draw_backtrack_skeletons(C_CSPlayer* local, C_CSPlayer* entity, Color& color)
{
	if (!WeaponConfigVars::get().WeaponConfig[g::weaponconfig].resolver.g_bBackTracking)
		return;
	
	if (local->GetTeam() == entity->GetTeam())
		return;

	if (Accuracy::get().Players[entity->EntIndex()].m_LagRecord.size() < 2)
		return;

	const auto studio_model = MODELINFO->GetStudioModel(entity->GetModel());

	if (!studio_model)
		return;

	auto& lastrecord = Accuracy::get().GetLastValidRecord(entity->EntIndex());

	if (!lastrecord.m_bMatrixBuilt)
		return;
	
	for (auto j = 0; j < studio_model->numbones; j++)
	{
		const auto p_bone = studio_model->pBone(j);
		if (!p_bone || !(p_bone->flags & 256) || p_bone->parent == -1)
			continue;

		Vector v_bone_pos1;
		if (!Math::WorldToScreen(Vector(lastrecord.m_BoneMatrix[j][0][3], lastrecord.m_BoneMatrix[j][1][3], lastrecord.m_BoneMatrix[j][2][3]), v_bone_pos1))
			continue;

		Vector v_bone_pos2;
		if (!Math::WorldToScreen(Vector(lastrecord.m_BoneMatrix[p_bone->parent][0][3], lastrecord.m_BoneMatrix[p_bone->parent][1][3], lastrecord.m_BoneMatrix[p_bone->parent][2][3]), v_bone_pos2))
			continue;

		CRender::DrawLine(v_bone_pos1.x, v_bone_pos1.y, v_bone_pos2.x, v_bone_pos2.y, color);
	}
}

void entity_esp::draw_bounds(C_CSPlayer* entity, Color& color, const kBoxSettings box_mode)
{
	auto ent_client_renderable = entity->GetClientRenderable();

	if (!ent_client_renderable)
		return;

	Vector min, max;
	ent_client_renderable->GetRenderBounds(min, max);

	Vector points[] =
	{
		{ min.x, min.y, min.z },{ min.x, max.y, min.z },
		{ max.x, max.y, min.z },{ max.x, min.y, min.z },
		{ max.x, max.y, max.z },{ min.x, max.y, max.z },
		{ min.x, min.y, max.z },{ max.x, min.y, max.z }
	};

	const auto trans = entity->GetCoordinateFrame();
	Vector points_transformed[8];

	for (auto i = 0; i < 8; i++)
		Math::VectorTransform(points[i], trans, points_transformed[i]);

	Vector flb, brt, blb, frt, frb, brb, blt, flt;
	
	if (!Math::WorldToScreen(points_transformed[3], flb) || !Math::WorldToScreen(points_transformed[5], brt)
		|| !Math::WorldToScreen(points_transformed[0], blb) || !Math::WorldToScreen(points_transformed[4], frt)
		|| !Math::WorldToScreen(points_transformed[2], frb) || !Math::WorldToScreen(points_transformed[1], brb)
		|| !Math::WorldToScreen(points_transformed[6], blt) || !Math::WorldToScreen(points_transformed[7], flt))
		return;

	switch (box_mode)
	{
		case BOX_TWO_DIMENSIONAL:
		{
			Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

			float left = flb.x;        // left
			float top = flb.y;        // top
			float right = flb.x;    // right
			float bottom = flb.y;    // bottom

			for (int i = 1; i < 8; i++)
			{
				if (left > arr[i].x)
					left = arr[i].x;
				if (top > arr[i].y)
					top = arr[i].y;
				if (right < arr[i].x)
					right = arr[i].x;
				if (bottom < arr[i].y)
					bottom = arr[i].y;
			}

			CRender::DrawLineWithOutline(left, top, left, bottom, color);
			CRender::DrawLineWithOutline(left, bottom, right, bottom, color);
			CRender::DrawLineWithOutline(right, bottom, right, top, color);
			CRender::DrawLineWithOutline(right, top, left, top, color);

			return;
		}
		case BOX_THREE_DIMENSIONAL:
		{
			CRender::DrawLineWithOutline(blb, brb, color);
			CRender::DrawLineWithOutline(blb, blt, color);
			CRender::DrawLineWithOutline(blb, flb, color);
			CRender::DrawLineWithOutline(flb, frb, color);
			CRender::DrawLineWithOutline(flb, flt, color);
			CRender::DrawLineWithOutline(brb, brt, color);
			CRender::DrawLineWithOutline(brb, frb, color);
			CRender::DrawLineWithOutline(frb, frt, color);
			CRender::DrawLineWithOutline(blt, brt, color);
			CRender::DrawLineWithOutline(blt, flt, color);
			CRender::DrawLineWithOutline(brt, frt, color);
			CRender::DrawLineWithOutline(flt, frt, color);
			return;
		}

		case BOX_CORNERS:
		{
			Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

			auto left = flb.x;        // left
			auto top = flb.y;        // top
			auto right = flb.x;    // right
			auto bottom = flb.y;    // bottom

			for (auto i = 1; i < 8; i++)
			{
				if (left > arr[i].x)
					left = arr[i].x;
				if (top > arr[i].y)
					top = arr[i].y;
				if (right < arr[i].x)
					right = arr[i].x;
				if (bottom < arr[i].y)
					bottom = arr[i].y;
			}

			CRender::DrawCornerESP(left, right, top, bottom, color);
			return;
		}
	default: ;
	}
}

void entity_esp::draw_hitboxes(C_CSPlayer* entity, Color& color, matrix3x4_t* matrix)
{
	if (!entity)
		return;

	const auto hdr = MODELINFO->GetStudioModel(entity->GetModel());
	if (!hdr) return;

	const auto set = hdr->GetHitboxSet(entity->GetPlayerHitboxSet());
	if (!set) return;

	for (auto i = 0; i < set->numhitboxes; i++)
	{
		const auto hitbox = set->pHitbox(i);
		if (!hitbox) return;

		const auto radius = hitbox->radius;
		auto bbmin = hitbox->bbmin,
			bbmax = hitbox->bbmax;

		if (hitbox->radius != -1.f)
		{
			bbmin -= radius;
			bbmax += radius;
		}

		bbmin *= WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_flPointScale;
		bbmax *= WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_flPointScale;

		std::vector<Vector> points =
		{
			Vector(bbmin.x, bbmin.y, bbmin.z), //lbb
			Vector(bbmin.x, bbmin.y, bbmax.z), //lbf
			Vector(bbmin.x, bbmax.y, bbmin.z), //ltb
			Vector(bbmin.x, bbmax.y, bbmax.z), //ltf

			Vector(bbmax.x, bbmin.y, bbmin.z), //rbb
			Vector(bbmax.x, bbmin.y, bbmax.z), //rbf
			Vector(bbmax.x, bbmax.y, bbmin.z), //rtb
			Vector(bbmax.x, bbmax.y, bbmax.z), //rtf
		};

		Vector hitbox_points[8];
		for (int index = 0; index < 8; index++)
			Math::VectorTransform(points[index], matrix[hitbox->bone], hitbox_points[index]);

		Vector lbb, lbf, ltb, ltf, rtb, rtf, rbb, rbf;
		if (!Math::WorldToScreen(hitbox_points[0], lbb) || !Math::WorldToScreen(hitbox_points[1], lbf) ||
			!Math::WorldToScreen(hitbox_points[2], ltb) || !Math::WorldToScreen(hitbox_points[3], ltf) ||
			!Math::WorldToScreen(hitbox_points[4], rbb) || !Math::WorldToScreen(hitbox_points[5], rbf) ||
			!Math::WorldToScreen(hitbox_points[6], rtb) || !Math::WorldToScreen(hitbox_points[7], rtf))
			return;

		CRender::DrawLine(lbb, lbf, color);
		CRender::DrawLine(lbb, ltb, color);
		CRender::DrawLine(ltb, ltf, color);
		CRender::DrawLine(ltf, lbf, color);
		CRender::DrawLine(lbf, rbf, color);
		CRender::DrawLine(rbf, rbb, color);
		CRender::DrawLine(lbb, rbb, color);
		CRender::DrawLine(rbb, rtb, color);
		CRender::DrawLine(rtb, rtf, color);
		CRender::DrawLine(rtf, rbf, color);
		CRender::DrawLine(ltb, rtb, color);
		CRender::DrawLine(ltf, rtf, color);
	}
}

void entity_esp::draw_head(C_CSPlayer* entity, Color& color, matrix3x4_t* matrix)
{
	if (!entity)
		return;

	const auto hdr = MODELINFO->GetStudioModel(entity->GetModel());
	if (!hdr) return;

	const auto set = hdr->GetHitboxSet(entity->GetPlayerHitboxSet());
	if (!set) return;

	const auto hitbox = set->pHitbox(0);
	if (!hitbox) return;

	const auto radius = hitbox->radius;
	auto bbmin = hitbox->bbmin,
		   bbmax = hitbox->bbmax;

	if (hitbox->radius != -1.f)
	{
		bbmin -= radius;
		bbmax += radius;
	}

	bbmin *= WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_flPointScale;
	bbmax *= WeaponConfigVars::get().WeaponConfig[g::weaponconfig].ragebot.g_flPointScale;
	
	std::vector<Vector> points = 
	{ 
		Vector(bbmin.x, bbmin.y, bbmin.z), //lbb
		Vector(bbmin.x, bbmin.y, bbmax.z), //lbf
		Vector(bbmin.x, bbmax.y, bbmin.z), //ltb
		Vector(bbmin.x, bbmax.y, bbmax.z), //ltf

		Vector(bbmax.x, bbmin.y, bbmin.z), //rbb
		Vector(bbmax.x, bbmin.y, bbmax.z), //rbf
		Vector(bbmax.x, bbmax.y, bbmin.z), //rtb
		Vector(bbmax.x, bbmax.y, bbmax.z), //rtf
	};

	Vector hitbox_points[8];
	for (auto index = 0; index < 8; index++)
		Math::VectorTransform(points[index], matrix[hitbox->bone], hitbox_points[index]);

	Vector lbb, lbf, ltb, ltf, rtb, rtf, rbb, rbf;
	if (!Math::WorldToScreen(hitbox_points[0], lbb) || !Math::WorldToScreen(hitbox_points[1], lbf) ||
		!Math::WorldToScreen(hitbox_points[2], ltb) || !Math::WorldToScreen(hitbox_points[3], ltf) ||
		!Math::WorldToScreen(hitbox_points[4], rbb) || !Math::WorldToScreen(hitbox_points[5], rbf) ||
		!Math::WorldToScreen(hitbox_points[6], rtb) || !Math::WorldToScreen(hitbox_points[7], rtf))
		return;

	CRender::DrawLine(lbb, lbf, color);
	CRender::DrawLine(lbb, ltb, color);
	CRender::DrawLine(ltb, ltf, color);
	CRender::DrawLine(ltf, lbf, color);
	CRender::DrawLine(lbf, rbf, color);
	CRender::DrawLine(rbf, rbb, color);
	CRender::DrawLine(lbb, rbb, color);
	CRender::DrawLine(rbb, rtb, color);
	CRender::DrawLine(rtb, rtf, color);
	CRender::DrawLine(rtf, rbf, color);
	CRender::DrawLine(ltb, rtb, color);
	CRender::DrawLine(ltf, rtf, color);
}

void entity_esp::draw_bomb(C_CSPlayer* entity)
{
	if (!entity)
		return;//Null check 

	const auto ent_client_class = entity->GetClientClass();

	if (!ent_client_class)
		return;

	//We only want to iterate over players. Make sure the ClassID is correct
	if (ent_client_class->m_ClassID != CC4)
		return;

	const auto v_origin = entity->GetOrigin();

	if (v_origin.x == 0 && v_origin.y == 0 && v_origin.z == 0)
		return;

	Vector v_screen_origin;
	if (Math::WorldToScreen(v_origin, v_screen_origin))
	{
		auto weapon = reinterpret_cast<CBaseCombatWeapon*>(entity);
		if (!weapon)
			return;

		if (VisualVars::get().ESP.g_bShowBomb == ICON_TEXT)
			CRender::get().DrawText(v_screen_origin.x, v_screen_origin.y, Color::GreenYellow(), true, XorStr("BOMB"));
		else if (VisualVars::get().ESP.g_bShowBomb == ICON_ON)
			CRender::get().DrawIcon(v_screen_origin.x, v_screen_origin.y, Color::GreenYellow(), true, "%s", std::string(1, weapon->GetWeaponIcon()).c_str());
	}
}

void entity_esp::draw_item_esp(C_CSPlayer* entity)
{
	if (!entity)
		return;

	auto* weapon = reinterpret_cast<CBaseCombatWeapon*>(entity);

	if (!weapon)
		return;

	if (weapon->GetState() != WEAPON_NOT_CARRIED)
		return;

	auto color = Color::White();

	const auto origin = reinterpret_cast<C_CSPlayer*>(weapon)->GetOrigin();
	if (origin.x == 0 && origin.y == 0 && origin.z == 0)
			return;

	std::string name = weapon->GetClientClass()->m_pNetworkName;
	
	if (name[0] == 'C')
	{
		if (name[1] == 'W')
		{
			name = name.substr(7, name.size() - 7);
		}
		else
		{
			name = name.substr(1, name.size() - 1);
		}
	}

	std::string gren = XorStr("Grenade");
	const auto start_position_to_erase = name.find(gren);

	if (start_position_to_erase != std::string::npos)
		name.erase(start_position_to_erase, gren.length());
	

	Vector origin_screen;
	if (!Math::WorldToScreen(origin, origin_screen))
		return;

	if (VisualVars::get().ESP.g_bShowGroundWeapons == ICON_TEXT)
		CRender::get().DrawText(origin_screen.x, origin_screen.y, Color::White(), true, "%s", name.c_str());
	else if (VisualVars::get().ESP.g_bShowGroundWeapons == ICON_ON)
		CRender::get().DrawIcon(origin_screen.x, origin_screen.y, Color::White(), true, "%s", std::string(1, weapon->GetWeaponIcon()).c_str());

	draw_bounds(entity, color, VisualVars::get().ESP.g_kGroundWeaponBoxMode);
}
#include "Chams.h"
#include "Globals.h"
#include "Accuracy.h"


void Chams::force_matrial(IMaterial* mat, Color color)
{
	float base[3];
	color.Base(base);
	RENDERVIEW->SetColorModulation(base);
	MODELRENDER->ForcedMaterialOverride(mat);
}

void Chams::player_chams(C_CSPlayer* local, IVModelRender* ecx, void* context, void* state, const ModelRenderInfo_t& info, matrix3x4_t* p_custom_bone_to_world, bool& shouldntdraw) const
{
	return;
	//get entity base pointer
	auto model_entity = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(info.entity_index));

	if (!model_entity) return;
	if (model_entity->IsDormant()) return;
	if (!model_entity->IsAlive()) return;

	std::string model_name = MODELINFO->GetModelName(info.pModel);

	if ((model_entity->GetTeam() == local->GetTeam() && !VisualVars::get().Chams.g_bChamsShowTeammates) && model_entity != local && VisualVars::get().g_bChams) { // we dont want to draw the model 
		shouldntdraw = true;																																	   // here if we're drawing it else where
		return;
	}

	if (VisualVars::get().Removals.g_bNoTeammates && model_entity->GetTeam() == local->GetTeam() && model_entity != local) {
		shouldntdraw = true;
		return;
	}
	//if thirdperson is enabled and model = local players model
	if (MiscVars::get().g_bThirdPerson && info.entity_index == local->EntIndex() && local->IsAlive() && !local->IsDormant())
	{
		static auto local_mat = MATSYSTEM->FindMaterial(XorStr("models/inventory_items/trophy_majors/crystal_clear"), XorStr(TEXTURE_GROUP_OTHER));
		static auto fake_mat = MATSYSTEM->FindMaterial(XorStr("models/inventory_items/dogtags/dogtags_outline"), XorStr(TEXTURE_GROUP_OTHER));

		if (!fake_mat || !local_mat || fake_mat->IsErrorMaterial() || local_mat->IsErrorMaterial())
			return;

		const auto showfakelagmodel_enabled = MiscVars::get().g_nFakeLag && MiscVars::get().g_bShowFakeLagModel && MiscVars::get().g_bThirdPerson && local->GetVelocity().Length() > 1.f;

		if (AAVars::get().g_bShowFakeAngle || AAVars::get().g_bShowLBYAngle || showfakelagmodel_enabled)
		{
			matrix3x4_t bone_to_world[MAXSTUDIOBONES];
			if (local->SetupBones(bone_to_world, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.f))
			{
				if (AAVars::get().g_bShowFakeAngle || showfakelagmodel_enabled)
				{
					const auto aa_angle = g::anti_aim_fake_angles.y - g::anti_aim_angles.y;

					Vector out_pos;
					matrix3x4_t bone_matrix[MAXSTUDIOBONES];
					for (auto i = 0; i < MAXSTUDIOBONES; i++)
					{
						Math::AngleMatrix(Vector(0, aa_angle, 0), bone_matrix[i]);
						Math::MatrixMultiply(bone_matrix[i], bone_to_world[i]);
						Math::VectorRotate(Vector(bone_to_world[i][0][3], bone_to_world[i][1][3], bone_to_world[i][2][3]) - info.origin, Vector(0, aa_angle, 0), out_pos);

						out_pos += (showfakelagmodel_enabled ? g::previous_position : info.origin);

						bone_matrix[i][0][3] = out_pos.x;
						bone_matrix[i][1][3] = out_pos.y;
						bone_matrix[i][2][3] = out_pos.z;
					}

					mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
					mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
					mat->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
					force_matrial(fake_mat, Color::Lime());
					original_func(ecx, context, state, info, bone_matrix);
					force_matrial();
				}

				if (AAVars::get().g_bShowLBYAngle)
				{
					const auto lby_angle = local->GetLowerBodyYawTarget() - g::anti_aim_angles.y;

					Vector out_pos2;
					matrix3x4_t bone_matrix2[MAXSTUDIOBONES];
					for (auto i = 0; i < MAXSTUDIOBONES; i++)
					{
						Math::AngleMatrix(Vector(0, lby_angle, 0), bone_matrix2[i]);
						Math::MatrixMultiply(bone_matrix2[i], bone_to_world[i]);
						Math::VectorRotate(Vector(bone_to_world[i][0][3], bone_to_world[i][1][3], bone_to_world[i][2][3]) - info.origin, Vector(0, lby_angle, 0), out_pos2);

						out_pos2 += info.origin;

						bone_matrix2[i][0][3] = out_pos2.x;
						bone_matrix2[i][1][3] = out_pos2.y;
						bone_matrix2[i][2][3] = out_pos2.z;
					}

					mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
					mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
					mat->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
					force_matrial(fake_mat, Color::Orange());
					original_func(ecx, context, state, info, bone_matrix2);
					force_matrial();
				}

				if (AAVars::get().g_bShowFakeAngle || AAVars::get().g_bShowLBYAngle)
					MODELRENDER->ForcedMaterialOverride(local_mat);
			}
		}
	}

	if (info.entity_index > 0 && info.entity_index <= 64 && model_entity != local)
	{
		auto& Player = Accuracy::get().Players[info.entity_index];

		if (Player.m_bUpdated) {
			if(MenuVars::get().g_bResolveYaw && false) {
				Vector old_eye = model_entity->GetEyeAngles(), old_abs_ang = model_entity->GetAbsAngles();

				Record &restore = Player.m_LagRecord[0];
				QAngle angle = restore.m_angAngles;

				bool nospread = false;
				if (nospread) {
					const int pitch = (int)floor(Player.m_nBruteforce / 8.f);
					static const int pitches[3] = { 89, -89, 0 };
					angle.x = pitches[pitch];
				}
				if (restore.m_nLbyUpdated) {
					angle.y = restore.m_flLowerBodyYawTarget + ((restore.m_nLbyUpdated == 2) ? 35 * Player.m_nLbyCorrect : 0);
				}
				else {
					angle.y = restore.m_flLowerBodyYawTarget;
					static const int yaws[8] = { 180, 0, 90, -90, 45, -135, 135, -45 };
					const int yaw = Player.m_nBruteforce % 8;
					angle.y += yaws[yaw];
				}

				model_entity->SetAbsAngles(angle);
				model_entity->GetEyeAngles() = angle;

				model_entity->InvalidateBoneCache();
				matrix3x4_t bones[128];
				model_entity->SetupBones(bones, 128, 128, 0);
				original_func(ecx, context, state, info, bones);
				model_entity->SetAbsAngles(old_abs_ang);
				model_entity->GetEyeAngles() = old_eye;
				model_entity->InvalidateBoneCache();
				shouldntdraw = true;
			}
			if(WeaponConfigVars::get().WeaponConfig[g::weaponconfig].resolver.g_bBackTracking && VisualVars::get().Chams.g_bBacktrackChams) { //TODO: add toggle
				auto& record = Accuracy::get().GetLastValidRecord(info.entity_index);
				original_func(ecx, context, state, info, record.m_BoneMatrix);
			}
		}
	}
}

void Chams::weapons_normal()
{
	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
	force_matrial(mat, Color(VisualVars::get().Chams.g_cVisibleAllyColor));
}

void Chams::weapons_wireframe()
{
	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
	mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
	force_matrial(mat, Color(VisualVars::get().Chams.g_cVisibleAllyColor));
}

void Chams::weapons_remove(std::string modelName)
{
	if (modelName.find(XorStr("glove")) != std::string::npos) //dont remove the glove
		return;

	IMaterial* arms_material = MATSYSTEM->FindMaterial(modelName.c_str(), XorStr(TEXTURE_GROUP_MODEL));

	if (!arms_material)
		return;

	arms_material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
	arms_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	arms_material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
	arms_material->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
	MODELRENDER->ForcedMaterialOverride(arms_material);
}

void Chams::weapons_chams(C_CSPlayer* pLocal, std::string modelName)
{
	if (!pLocal)
		return;

	if (INPUT->m_fCameraInThirdPerson)
		return;

	if (modelName.find(XorStr("arms")) != std::string::npos)
		return;

	if (modelName.find(XorStr("glove")) != std::string::npos) //dont remove the sleeve
		return;

	CBaseCombatWeapon* weapon = pLocal->GetActiveWeapon();

	if (!weapon)
		return;

	if (weapon->GetId() == weapon_c4)
		return;

	if (MiscVars::get().g_bFOVChanger && weapon->GetWeaponType() == WEAPONTYPE_SNIPER && weapon->GetZoomLevel() != 0) //remove weapon while zoomed in
		return weapons_remove(modelName);

	if (!VisualVars::get().Chams.g_iChamsWeapon)
		return;

	switch (VisualVars::get().Chams.g_iChamsWeapon)
	{
	case CHAMS_NORMAL:
		weapons_normal();
		break;
	case CHAMS_WIREFRAME:
		weapons_wireframe();
		break;
	case CHAMS_REMOVE:
		weapons_remove(modelName);
		break;
	default: break;
	}
}

void Chams::arms_normal()
{
	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
	force_matrial(mat, Color(VisualVars::get().Chams.g_cVisibleAllyColor));
}

void Chams::arms_wireframe()
{
	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
	mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
	force_matrial(mat, Color(VisualVars::get().Chams.g_cVisibleAllyColor));
}

void Chams::arms_remove(std::string modelName)
{
	if (modelName.find(XorStr("glove")) != std::string::npos) //dont remove the glove
		return;

	IMaterial* arms_material = MATSYSTEM->FindMaterial(modelName.c_str(), XorStr(TEXTURE_GROUP_MODEL));

	if (!arms_material)
		return;

	arms_material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
	arms_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	arms_material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
	arms_material->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
	MODELRENDER->ForcedMaterialOverride(arms_material);
}

void Chams::arms_chams(std::string modelName)
{
	if (!VisualVars::get().Chams.g_iChamsArms)
		return;

	if (INPUT->m_fCameraInThirdPerson)
		return;

	if (modelName.find(XorStr("glove")) != std::string::npos) //dont remove the sleeve
		return;

	switch (VisualVars::get().Chams.g_iChamsArms)
	{
	case CHAMS_NORMAL:
		arms_normal();
		break;
	case CHAMS_WIREFRAME:
		arms_wireframe();
		break;
	case CHAMS_REMOVE:
		arms_remove(modelName);
		break;
	default: break;
	}
}

void Chams::gloves_normal()
{
	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
	force_matrial(mat, Color(VisualVars::get().Chams.g_cVisibleAllyColor));
}

void Chams::gloves_wireframe()
{
	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
	mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
	force_matrial(mat, Color(VisualVars::get().Chams.g_cVisibleAllyColor));
}

void Chams::gloves_remove(std::string modelName)
{
	IMaterial* gloves_material = MATSYSTEM->FindMaterial(modelName.c_str(), XorStr(TEXTURE_GROUP_MODEL));

	if (!gloves_material)
		return;

	gloves_material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
	gloves_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	gloves_material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
	gloves_material->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);
	MODELRENDER->ForcedMaterialOverride(gloves_material);
}

void Chams::gloves_chams(std::string modelName)
{
	if (!VisualVars::get().Chams.g_iChamsGloves)
		return;

	if (INPUT->m_fCameraInThirdPerson)
		return;

	if (modelName.find(XorStr("sleeve")) != std::string::npos) //dont remove the sleeve
		return;

	switch (VisualVars::get().Chams.g_iChamsGloves)
	{
	case CHAMS_NORMAL:
		gloves_normal();
		break;
	case CHAMS_WIREFRAME:
		gloves_wireframe();
		break;
	case CHAMS_REMOVE:
		gloves_remove(modelName);
		break;
	default: break;
	}
}

void Chams::draw_model_execute(DrawModelExecute_t oFunc, IVModelRender* ecx, void* context, void* state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld, bool& shouldntdraw)
{
	original_func = oFunc;

	if (ENGINE->IsTakingScreenshot() || !ENGINE->IsInGame())
		return;

	if (!info.pModel)
		return;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (!mat)
		mat = MATSYSTEM->CreateMaterial(false, false, false);

	//reset the material flags to default ^
	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
	mat->SetMaterialVarFlag(MATERIAL_VAR_FLAT, false);

	std::string modelName = MODELINFO->GetModelName(info.pModel);

	if (modelName.find(XorStr("models/player")) != std::string::npos)
	{
		player_chams(pLocal, ecx, context, state, info, pCustomBoneToWorld, shouldntdraw);
	}
	else if (modelName.find(XorStr("arms")) != std::string::npos)
	{
		gloves_chams(modelName);
		arms_chams(modelName);
	}
	else if (modelName.find(XorStr("weapon")) != std::string::npos)
	{
		weapons_chams(pLocal, modelName);
	}
}

void Chams::scene_end()
{
	if (!VisualVars::get().g_bChams)
		return;

	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
		return;

	C_CSPlayer* local = C_CSPlayer::GetLocalPlayer();

	if (!local)
		return;

	static auto mat_hidden = MATSYSTEM->CreateMaterial(false, true, false);
	static auto mat_visible = MATSYSTEM->CreateMaterial(false, false, false);

	if (!mat_hidden || !mat_visible)
		return;

	Color render_color_hidden_ally = VisualVars::get().Chams.g_cHiddenAllyColor;
	float color_hidden_ally[3];
	render_color_hidden_ally.Base(color_hidden_ally);

	Color render_color_hidden_enemy = VisualVars::get().Chams.g_cHiddenEnemyColor;
	float color_hidden_enemy[3];
	render_color_hidden_enemy.Base(color_hidden_enemy);

	Color render_color_visible_ally = VisualVars::get().Chams.g_cVisibleAllyColor;
	float color_visible_ally[3];
	render_color_visible_ally.Base(color_visible_ally);

	Color render_color_visible_enemy = VisualVars::get().Chams.g_cVisibleEnemyColor;
	float color_visible_enemy[3];
	render_color_visible_enemy.Base(color_visible_enemy);

	for (auto i = 1; i <= 64; i++)
	{
		auto ent = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));

		if (!ent) continue;
		if (ent == local) continue;
		if (ent->IsDormant()) continue;
		if (!ent->IsAlive()) continue;
		if (!ent->IsPlayer()) continue;

		if (!VisualVars::get().Chams.g_bChamsShowTeammates && ent->GetTeam() == local->GetTeam())
			continue;

		const auto color_hidden = (ent->GetTeam() == local->GetTeam()) ? color_hidden_ally : color_hidden_enemy;
		const auto color_visible = (ent->GetTeam() == local->GetTeam()) ? color_visible_ally : color_visible_enemy;

		if (VisualVars::get().g_bWallhack) //draw through walls
		{
			force_matrial(mat_hidden, color_hidden);
			ent->DrawModel(STUDIO_RENDER, 255);
			force_matrial();
		}

		if (!Utils::LineGoesThroughSmoke(local->GetEyePos(), ent->GetEyePos())) //TODO: use pelvis pos so we dont draw on edges of smoke when we cant normally see the player
		{
			force_matrial(mat_visible, color_visible);
			ent->DrawModel(STUDIO_RENDER, 255);
			force_matrial();
		}

		//auto& player = Accuracy::get().Players[i];

		//if (player.m_bUpdated && !player.m_LagRecord.empty()) {
		//	auto& record = Accuracy::get().GetLastValidRecord(i);
		//	Accuracy::BacktrackPlayerToTick(ent, player, record);
		//	ent->InvalidateBoneCache();
		//	//draw
		//	force_matrial(mat_hidden, color_hidden);
		//	ent->DrawModel(STUDIO_RENDER, 100);
		//	force_matrial();

		//	Accuracy::RestorePlayer(ent, player);
		//	ent->InvalidateBoneCache();
		//}

	}
}

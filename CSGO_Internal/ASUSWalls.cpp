#include "ASUSWalls.h"
#include "Globals.h"
#include "SourceEngine/IMaterial.h"
#include "CVarManager.h"

void ASUSWalls::SpoofDrawStaticProps()
{
	if (!CVarManager::get().DrawSpecificStaticProp)
		CVarManager::get().DrawSpecificStaticProp = CVAR->FindVar(XorStr("r_DrawSpecificStaticProp"));

	if (!CVarManager::get().DrawSpecificStaticProp)
		return;

	if (CVarManager::get().DrawSpecificStaticProp->GetInt() != 0)
		CVarManager::get().DrawSpecificStaticProp->SetValue(0);
}

bool ASUSWalls::ChangeMaterial(IMaterial* mat, float alpha)
{
	if (!mat)
		return false;

	mat->AlphaModulate(alpha);

	return true;
}

void ASUSWalls::RestoreMaterials()
{
	for (auto mathandle = MATSYSTEM->FirstMaterial(); mathandle != MATSYSTEM->InvalidMaterial(); mathandle = MATSYSTEM->NextMaterial(mathandle))
	{
		if (!mathandle)
			continue;

		auto mat = MATSYSTEM->GetMaterial(mathandle);

		if (!mat)
			continue;

		if (!strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_STATIC_PROP))
			ChangeMaterial(mat, 1.f);
	}

	asuswallsupdated = true;
}

void ASUSWalls::FrameStageNotify(ClientFrameStage_t stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (asuswallsupdated)
		return;

	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
	{
		asuswallsupdated = false;
		return;
	}

	if (!VisualVars::get().g_bASUSWalls)
	{
		RestoreMaterials();
		return;
	}

	SpoofDrawStaticProps();

	for (auto mathandle = MATSYSTEM->FirstMaterial(); mathandle != MATSYSTEM->InvalidMaterial(); mathandle = MATSYSTEM->NextMaterial(mathandle))
	{
		if (!mathandle)
			continue;

		auto mat = MATSYSTEM->GetMaterial(mathandle);

		if (!mat)
			continue;

		if (!strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_STATIC_PROP))
			ChangeMaterial(mat, VisualVars::get().g_fASUSWallsScale);
	}
}

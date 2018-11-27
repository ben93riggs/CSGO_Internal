#include "WorldChams.h"
#include "Globals.h"
#include "SourceEngine/IMaterial.h"
#include "CVarManager.h"

void WorldChams::SpoofDrawStaticProps()
{
	if (!CVarManager::get().DrawSpecificStaticProp)
		CVarManager::get().DrawSpecificStaticProp = CVAR->FindVar(XorStr("r_DrawSpecificStaticProp"));

	if (!CVarManager::get().DrawSpecificStaticProp)
		return;

	if (CVarManager::get().DrawSpecificStaticProp->GetInt() != 0)
		CVarManager::get().DrawSpecificStaticProp->SetValue(0);
}

bool WorldChams::ChangeMaterial(IMaterial* mat, float red, float green, float blue)
{
	if (mat == nullptr)
		return false;

	//float r, g, b;
	//mat->GetColorModulation(&r, &g, &b);

	mat->ColorModulate(red, green, blue);


	return true;
}

void WorldChams::RestoreMaterials()
{
	for (auto mathandle = MATSYSTEM->FirstMaterial(); mathandle != MATSYSTEM->InvalidMaterial(); mathandle = MATSYSTEM->NextMaterial(mathandle))
	{
		if (!mathandle)
			continue;

		auto mat = MATSYSTEM->GetMaterial(mathandle);

		if (!mat)
			continue;

		if (!strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_WORLD) || !strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_STATIC_PROP))
			ChangeMaterial(mat, 1, 1, 1);
	}

	nightmodeupdated = true;
}

void WorldChams::FrameStageNotify(ClientFrameStage_t stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (nightmodeupdated)
		return;

	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
	{
		nightmodeupdated = false;
		return;
	}

	if (!VisualVars::get().g_bWorldModulation)
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

		if (!strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_WORLD) || !strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_STATIC_PROP))
			ChangeMaterial(mat, VisualVars::get().g_cWorldModulationColor[0], VisualVars::get().g_cWorldModulationColor[1], VisualVars::get().g_cWorldModulationColor[2]);
	}

	nightmodeupdated = true;
}

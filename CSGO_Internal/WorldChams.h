#pragma once
#include "Singleton.h"
#include "SourceEngine/Definitions.hpp"

class IMaterial;

class WorldChams : public singleton<WorldChams>
{
	static void SpoofDrawStaticProps();
	static bool ChangeMaterial(IMaterial* mat, float red, float green, float blue);
	void RestoreMaterials();

public:
	bool nightmodeupdated = true;
	void FrameStageNotify(ClientFrameStage_t stage);
};




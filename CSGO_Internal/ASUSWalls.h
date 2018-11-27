#pragma once
#include "Singleton.h"
#include "SourceEngine/Definitions.hpp"

class IMaterial;

class ASUSWalls : public singleton<ASUSWalls>
{
	static void SpoofDrawStaticProps();
	static bool ChangeMaterial(IMaterial* mat, float alpha);
	void RestoreMaterials();

public:
	bool asuswallsupdated = true;
	void FrameStageNotify(ClientFrameStage_t stage);
};

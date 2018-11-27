#pragma once
#include "Singleton.h"
#include "SourceEngine/Definitions.hpp"

class C_CSPlayer;

class DisableInterpolation : public singleton<DisableInterpolation>
{
public:
	static void FrameStageNotify(ClientFrameStage_t stage);
};

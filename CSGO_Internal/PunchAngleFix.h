#pragma once
#include "Singleton.h"
#include "SourceEngine/QAngle.hpp"
#include "BHop.h"

class PunchAngleFix : public singleton<PunchAngleFix>
{
	QAngle m_aimPunchAngle[128];

public:

	void PunchAngleFix_RunCommand(C_CSPlayer* base_player);
	void PunchAngleFix_FSN(ClientFrameStage_t stage);
};


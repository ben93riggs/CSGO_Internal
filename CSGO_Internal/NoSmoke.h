#pragma once
#include "Singleton.h"
#include "SourceEngine/Definitions.hpp"

class NoSmoke : public singleton<NoSmoke>
{
	unsigned long smoke_count_offset = 0;

public:
	void FrameStageNotify(ClientFrameStage_t stage);
};

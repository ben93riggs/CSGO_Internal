#pragma once
#include "Singleton.h"
#include "SourceEngine/CInput.hpp"
#include "AALines.h"

class closest_to_crosshair : public singleton<closest_to_crosshair>
{
public:
	static bool is_valid_target(C_CSPlayer* ent, C_CSPlayer* p_local);
	static void run(CUserCmd* cmd);
};

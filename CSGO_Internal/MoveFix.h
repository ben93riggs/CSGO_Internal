#pragma once
#include "Singleton.h"
#include "SourceEngine/CInput.hpp"
//#include "SourceEngine/CInput.hpp"

class CUserCmd;

class MoveFix : public singleton<MoveFix>
{
public:
	static void Start(CUserCmd* pCmd);

	void End(CUserCmd* pCmd) const;
	void Fix(CUserCmd* user_cmd, const QAngle& vector) const;
};

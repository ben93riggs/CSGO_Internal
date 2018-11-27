#pragma once
#include "Singleton.h"

class AutoPistol : public singleton<AutoPistol>
{
public:
	static void CreateMove();
	
};

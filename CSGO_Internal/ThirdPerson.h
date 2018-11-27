#pragma once
#include "Singleton.h"

class CViewSetup;

class ThirdPerson : public singleton<ThirdPerson>
{
public:
	void OverrideView(CViewSetup* vsView) const;
};

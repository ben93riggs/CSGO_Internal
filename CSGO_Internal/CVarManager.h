#pragma once
#include "Singleton.h"
#include "SourceEngine/Convar.h"

class CVarManager : public singleton<CVarManager>
{
public:
	ConVar * DrawSpecificStaticProp = nullptr;
};
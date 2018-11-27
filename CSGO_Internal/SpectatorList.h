#pragma once
#include "Globals.h"

class SpectatorList : public singleton<SpectatorList>
{
	static void Run();

public:

	void EndScene() const;
};
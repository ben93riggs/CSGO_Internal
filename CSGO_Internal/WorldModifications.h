#pragma once
#include "Singleton.h"

class WorldModifications : public singleton<WorldModifications>
{
public:
	static void Greymode();
	static void Fullbright();
	static void Psychedelic();
	static void Pixelated();
	static void PostProcessing();
};


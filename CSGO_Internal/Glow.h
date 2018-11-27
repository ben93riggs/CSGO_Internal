#pragma once
#include "GlowManager.h"
#include "Singleton.h"

class Glow : public singleton<Glow>
{
	float weaponColor[4] = { 255.f / 255.f, 138.f / 255.f, 46.f / 255.f };
	float c4Color[4] = { 84.f / 255.f, 147.f / 255.f, 230.f / 255.f };
	float chickenColor[4] = { 255.f / 255.f, 255.f / 255.f, 255.f / 255.f };
	float grenadeColor[4] = { 230.f / 255.f, 78.f / 255.f, 255.f / 255.f };

	CGlowObjectManager* GlowObjectManager = nullptr;

public:
	void DoPostScreenSpaceEffects();
};

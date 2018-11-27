#pragma once
#include "Singleton.h"


class AutoWallCrosshair : public singleton<AutoWallCrosshair>
{
public:
	void PaintTraverse(float centerx, float centery) const;
};

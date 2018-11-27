#pragma once
#include "Singleton.h"

class CRender;
class C_CSPlayer;

class RecoilCrosshair : public singleton<RecoilCrosshair>
{
public:
	void PaintTraverse(C_CSPlayer* pLocal, int iScreenHeight, int iScreenWidth, int centerx, int centery) const;
};

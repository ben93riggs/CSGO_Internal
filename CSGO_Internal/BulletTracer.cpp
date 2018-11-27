#include "BulletTracer.h"
#include "SourceEngine/SDK.hpp"

void BulletBeams::DrawBeam(Vector src, Vector dst, Color col)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS;
	beamInfo.m_pszModelName = XorStr("sprites/physbeam.vmt");
	beamInfo.m_nModelIndex = -1; // will be set by CreateBeamPoints if its -1
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = 1.5f;
	beamInfo.m_flWidth = 2.0f;
	beamInfo.m_flEndWidth = 2.0f;
	beamInfo.m_flFadeLength = 0.f;
	beamInfo.m_flAmplitude = 2.0f;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.2f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = col.GetR();
	beamInfo.m_flGreen = col.GetG();
	beamInfo.m_flBlue = col.GetB();
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = 0;

	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = dst;

	Beam_t* myBeam = VIEWRENDERBEAM->CreateBeamPoints(beamInfo);

	if (myBeam)
		VIEWRENDERBEAM->DrawBeam(myBeam);
}

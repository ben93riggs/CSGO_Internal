#include "ThirdPerson.h"
#include "SourceEngine/SDK.hpp"
#include "Config.h"
#include "Math.hpp"

class CTraceFilterNoPlayers : public CTraceFilter
{
public:
	CTraceFilterNoPlayers() {}
	bool ShouldHitEntity(IHandleEntity *pEntity, int contentsMask) override
	{
		IClientUnknown *pUnk = (IClientUnknown*)pEntity;

		if (!pUnk)
			return false;

		IClientEntity *pEnt = pUnk->GetIClientEntity();

		if (!pEnt)
			return false;

		ClientClass* ent_client_class = pEnt->GetClientClass();

		if (!ent_client_class)
			return false;

		return ent_client_class->m_ClassID != CCSPlayer;
	}
};

const Vector mins = Vector(-16.f, -16.f, -16.f);
const Vector maxs = Vector(16.f, 16.f, 16.f);

void ThirdPerson::OverrideView(CViewSetup* vsView) const
{
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	Vector angles;
	ENGINE->GetViewAngles(angles);

	if (MiscVars::get().g_bThirdPerson && !ENGINE->IsTakingScreenshot())
	{
		Vector vecForward;

		INPUT->m_fCameraInThirdPerson = true;

		Vector localEyePos = pLocal->GetEyePos();

		Vector cam_offset = Vector(angles.x, angles.y, MiscVars::get().g_iThirdPersonFOV);

		Math::AngleVectors(Vector(cam_offset.x, cam_offset.y, 0.f), &vecForward);

		CTraceFilterNoPlayers filter;
		CGameTrace tr;
		Ray_t ray(localEyePos, localEyePos - (vecForward * cam_offset.z), mins, maxs);

		ENGINETRACE->TraceRay(ray, MASK_SHOT_HULL, &filter, &tr);

		cam_offset.z *= tr.fraction;

		Math::AngleVectors(Vector(cam_offset.x, cam_offset.y, 0.f), &vecForward);

		vsView->origin = Vector(vsView->origin.x + -cam_offset.z * vecForward.x, vsView->origin.y + -cam_offset.z * vecForward.y, vsView->origin.z + MiscVars::get().g_iThirdPersonHeight);
		INPUT->m_vecCameraOffset = Vector(cam_offset.x, cam_offset.y, cam_offset.z);
	}
	else
	{
		if ( INPUT->m_fCameraInThirdPerson )
		{
			ENGINE->ExecuteClientCmd(XorStr("firstperson"));
			INPUT->m_fCameraInThirdPerson = false;
		}
	}
}

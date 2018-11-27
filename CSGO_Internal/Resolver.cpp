#include "Resolver.h"
#include "Globals.h"

bool Resolver::override(C_CSPlayer* local, C_CSPlayer* entity) const
{
	if (!local)
		return false;

	if (!entity)
		return false;

	Vector view_angles;
	ENGINE->GetViewAngles(view_angles);

	const auto i = entity->EntIndex();

	auto at_angles = Math::CalcAngle(local->GetEyePos(), entity->GetEyePos());
	Math::GUWOPNormalize(at_angles);

	auto delta_angle = view_angles - at_angles;
	Math::GUWOPNormalize(delta_angle);

	if (abs(delta_angle.y) > 10.f)
		return false;

	if (delta_angle.y > 0.f)
	{
		entity->GetEyeAngles().y = at_angles.y + 90.f;
		g::plist[i].overridevalue = 90.f;
	}
	else if (delta_angle.y < 0.f)
	{
		entity->GetEyeAngles().y = at_angles.y - 90.f;
		g::plist[i].overridevalue = -90.f;
	}

	g::plist[i].overridden = true;
	g::plist[i].corrections.bAimThisShot = false;

	return true;
}
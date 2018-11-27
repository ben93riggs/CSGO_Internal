#include "MoveFix.h"
#include "Globals.h"

void MoveFix::Start(CUserCmd* pCmd)
{
	//G::backup_cmd = *pCmd;
}

void MoveFix::End(CUserCmd * pCmd) const
{
	//QAngle currentAngles = pCmd->viewangles;
	//Math::GUWOPNormalize(currentAngles);
	//Math::ClampAngles(currentAngles);

	//float delta = currentAngles.y - G::m_vecStrafedAngles.y;
	//float oldYaw, currentYaw;

	//if (G::m_vecStrafedAngles.y < 0.f) {
	//	oldYaw = 360.0f + G::m_vecStrafedAngles.y;
	//}
	//else {
	//	oldYaw = G::m_vecStrafedAngles.y;
	//}

	//if (currentAngles.y < 0.0f) {
	//	currentYaw = 360.0f + currentAngles.y;
	//}
	//else {
	//	currentYaw = currentAngles.y;
	//}

	//if (currentYaw < oldYaw) {
	//	delta = abs(currentYaw - oldYaw);
	//}
	//else {
	//	delta = 360.0f - abs(oldYaw - currentYaw);
	//}

	//delta = 360.0f - delta;
	//pCmd->forwardmove = cos(DEG2RAD(delta)) * G::backup_cmd.forwardmove + cos(DEG2RAD(delta + 90.f)) * G::backup_cmd.sidemove;
	//pCmd->sidemove = sin(DEG2RAD(delta)) * G::backup_cmd.forwardmove + sin(DEG2RAD(delta + 90.f)) * G::backup_cmd.sidemove;
}

void MoveFix::Fix(CUserCmd* usercmd, const QAngle& wish_angle) const
{
	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto viewangles = usercmd->viewangles;
	viewangles.Normalize();

	Math::AngleVectors(wish_angle, view_fwd, view_right, view_up);
	Math::AngleVectors(viewangles, cmd_fwd, cmd_right, cmd_up);

	const float v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const float v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const float v12 = sqrtf(view_up.z * view_up.z);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const float v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const float v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const float v18 = sqrtf(cmd_up.z * cmd_up.z);

	const Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const Vector norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const float v22 = norm_view_fwd.x * usercmd->forwardmove;
	const float v26 = norm_view_fwd.y * usercmd->forwardmove;
	const float v28 = norm_view_fwd.z * usercmd->forwardmove;
	const float v24 = norm_view_right.x * usercmd->sidemove;
	const float v23 = norm_view_right.y * usercmd->sidemove;
	const float v25 = norm_view_right.z * usercmd->sidemove;
	const float v30 = norm_view_up.x * usercmd->upmove;
	const float v27 = norm_view_up.z * usercmd->upmove;
	const float v29 = norm_view_up.y * usercmd->upmove;

	usercmd->forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	usercmd->sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	usercmd->upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	usercmd->forwardmove = Math::clamp(usercmd->forwardmove, -450.f, 450.f);
	usercmd->sidemove = Math::clamp(usercmd->sidemove, -450.f, 450.f);
	usercmd->upmove = Math::clamp(usercmd->upmove, -320.f, 320.f);
}

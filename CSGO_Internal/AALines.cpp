#include "AALines.h"
#include "Globals.h"
#include "SourceEngine/Vector.hpp"
#include "CRender.h"

void AALines::Normalize360(float& angle)
{
	angle += 90.f;

	while (angle < 0.f)
		angle += 360.f;

	//angle += 180; //flip the angle

	while (angle > 360.f)
		angle -= 360.f;
}

void AALines::ScopedAALines(C_CSPlayer* pLocal) const
{
	//if (!pLocal->IsScoped())
	//return;

	int height;
	int width;
	ENGINE->GetScreenSize(width, height);

	Vector center(width * .5f, height * .5f, 0);

	Vector currentangles;
	ENGINE->GetViewAngles(currentangles);

	float current = currentangles.y;
	float lby = pLocal->GetLowerBodyYawTarget();
	float real = g::anti_aim_angles.y;
	float fake = g::anti_aim_fake_angles.y;

	Normalize360(current);
	Normalize360(lby);
	Normalize360(real);
	Normalize360(fake);

	float lby_relative = current - lby;
	float real_relative = current - real;
	float fake_relative = current - fake;

	Normalize360(lby_relative);
	Normalize360(real_relative);
	Normalize360(fake_relative);

	Vector lbypoint, realpoint, fakepoint;
	rotate_point(lbypoint, center, lby_relative, center.Length2D() * .075f);
	rotate_point(realpoint, center, real_relative, center.Length2D() * .075f - 40);
	rotate_point(fakepoint, center, fake_relative, center.Length2D() * .075f - 20);


	CRender::get().DrawFilledCircle(Vector2D(lbypoint.x, lbypoint.y), 32, 10, Color::Orange(75));
	CRender::get().DrawFilledCircle(Vector2D(fakepoint.x, fakepoint.y), 32, 10, Color::Green(75));
	CRender::get().DrawFilledCircle(Vector2D(realpoint.x, realpoint.y), 32, 10, Color::Red(75));

	CRender::get().DrawText(lbypoint.x, lbypoint.y, Color::White(150), true, "L");
	CRender::get().DrawText(fakepoint.x, fakepoint.y, Color::White(150), true, "F");
	CRender::get().DrawText(realpoint.x, realpoint.y, Color::White(150), true, "R");
}

void AALines::PaintTraverse(C_CSPlayer* pLocal) const
{
	if (!AAVars::get().g_bAAEnabled)
		return;

	if (!AAVars::get().g_bAALinesEnabled)
		return;

	if (!ENGINE->IsConnected() || !ENGINE->IsInGame())
		return;

	if (!pLocal->IsAlive())
		return;

	ScopedAALines(pLocal);
}

void AALines::rotate_point(Vector& point_out, Vector& c, float angle, float dist_from_center) const
{
	Vector p = c;
	p.x -= dist_from_center;

	float sinang = sin(DEG2RAD(angle));
	float cosang = cos(DEG2RAD(angle));

	p -= c;

	float xnew = p.x * cosang - p.y * sinang;
	float ynew = p.y * cosang + p.x * sinang;

	p.x = xnew + c.x;
	p.y = ynew + c.y;

	point_out = p;
}

void AALinesEnemy::PaintTraverse(C_CSPlayer * pLocal) const
{
	if (!AAVars::get().g_bEnemyAALinesEnabled)
		return;

	if (!ENGINE->IsConnected() || !ENGINE->IsInGame())
		return;

	if (!pLocal)
		return;

	for (int i = 0; i <= 64; i++)
	{
		if (i == pLocal->EntIndex())
			continue;

		C_CSPlayer* ent = static_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));

		if (!ent)
			continue;

		if (!ent->IsAlive())
			continue;

		if (ent->IsDormant())
			continue;

		if (ent->GetTeam() == pLocal->GetTeam())
			continue;

		if (ent->GetClientClass()->m_ClassID != CCSPlayer)
			continue;

		Vector forward, src, dst;
		trace_t tr;
		Ray_t ray;

		CTraceFilter filter;
		filter.pSkip = ent;

		Math::AngleVectors(QAngle(0, ent->GetLowerBodyYawTarget(), 0), &forward);

		Vector src3D = ent->GetEyePos();
		Vector dst3D = src3D + (forward * 35.f);

		ray.Init(src3D, dst3D);

		ENGINETRACE->TraceRay(ray, 0, &filter, &tr);

		if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
			continue;

		CRender::get().DrawLine(src.x, src.y, dst.x, dst.y, Color(210, 105, 30, 255));
		CRender::get().DrawCircle(dst.x, dst.y, 3, 32, Color(210, 105, 30, 255));
		CRender::get().DrawText(dst.x, dst.y, Color(210, 105, 30, 255), true, XorStr("lby"));

		/////////////////

		Math::AngleVectors(QAngle(0, g::plist[ent->EntIndex()].backupangle.y, 0), &forward);
		dst3D = src3D + (forward * 30.f);

		ray.Init(src3D, dst3D);

		ENGINETRACE->TraceRay(ray, 0, &filter, &tr);

		if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
			continue;

		CRender::get().DrawLine(src.x, src.y, dst.x, dst.y, Color(0, 255, 0, 255));
		CRender::get().DrawCircle(dst.x, dst.y, 3, 32, Color(0, 255, 0, 255));
		CRender::get().DrawText(dst.x, dst.y, Color(0, 255, 0, 255), true, XorStr("fake"));

		/////////////////////

		Math::AngleVectors(QAngle(0, ent->GetEyeAngles().y, 0), &forward);
		dst3D = src3D + (forward * 25.f);

		ray.Init(src3D, dst3D);

		ENGINETRACE->TraceRay(ray, 0, &filter, &tr);

		if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
			continue;

		CRender::get().DrawLine(src.x, src.y, dst.x, dst.y, Color(255, 0, 0, 255));
		CRender::get().DrawCircle(dst.x, dst.y, 3, 32, Color(255, 0, 0, 255));
		CRender::get().DrawText(dst.x, dst.y, Color(255, 0, 0, 255), true, XorStr("resolved"));
	}
}

#include "Freestanding.h"
#include "Globals.h"

struct angle_data {
	float angle;
	float thickness;
	angle_data(const float angle, const float thickness) : angle(angle), thickness(thickness) {}
};

float quick_normalize(float degree, const float min, const float max) {
	while (degree < min)
		degree += max - min;
	while (degree > max)
		degree -= max - min;

	return degree;
}

bool trace_to_exit_short(Vector &point, Vector &dir, const float step_size, float max_distance)
{
	float flDistance = 0;

	while (flDistance <= max_distance)
	{
		flDistance += step_size;

		point += dir * flDistance;

		if ((ENGINETRACE->GetPointContents(point) & MASK_SOLID) == 0)
		{
			// found first free point
			return true;
		}
	}

	return false;
}

float get_thickness(Vector& start, Vector& end) {
	Vector dir = end - start;
	Vector step = start;
	dir /= dir.Length();
	CTraceFilter filter;
	trace_t trace;
	Ray_t ray;
	float thickness = 0;
	while(true) {
		ray.Init(step, end);
		ENGINETRACE->TraceRay(ray, MASK_SOLID, &filter, &trace);
		
		if (!trace.DidHit())
			break;

		const Vector lastStep = trace.endpos;
		step = trace.endpos;

		if ((end - start).Length() <= (step - start).Length())
			break;

		if (!trace_to_exit_short(step, dir, 5, 90))
			return FLT_MAX;

		thickness += (step - lastStep).Length();
	}
	return thickness;
}

void freestanding::edge_anti_aim(CUserCmd *cmd) const {
	std::vector<angle_data> points;
	if (!AAVars::get().aa[g::i_current_aa].g_bFreeStanding)
		return;

	if (!cmd)
		return;

	const auto local = C_CSPlayer::GetLocalPlayer();

	if (!local)
		return;

	if (!local->IsAlive())
		return;

	const auto local_position = local->GetEyePos();
	std::vector<float> scanned = {};

	for (auto i = 0; i <= 64; i++) 
	{
		auto p_entity = dynamic_cast<C_CSPlayer*> (ENTITYLIST->GetClientEntity(i));
		if (p_entity == nullptr) continue;
		if (p_entity == local) continue;
		if (!p_entity->IsAlive()) continue;
		if (p_entity->GetTeam() == local->GetTeam()) continue;
		if (p_entity->IsDormant()) continue;
		if (!p_entity->IsPlayer()) continue;

		const auto view = Math::CalcAngle(local_position, p_entity->GetEyePos());

		std::vector<angle_data> angs;

		for (auto y = 1; y < 4; y++)
		{
			auto ang = quick_normalize((y * 90) + view.y, -180.f, 180.f);
			auto found = false; // check if we already have a similar angle

			for (auto i2 : scanned)
				if (abs(quick_normalize(i2 - ang, -180.f, 180.f)) < 20.f)
					found = true;

			if (found)
				continue;

			points.emplace_back( ang, -1.f );
			scanned.push_back(ang);
		}
		//points.push_back(base_angle_data(view.y, angs)); // base yaws and angle data (base yaw needed for lby breaking etc)
	}

	for (auto i = 0; i <= 64; i++) 
	{
		auto p_entity = dynamic_cast<C_CSPlayer*> (ENTITYLIST->GetClientEntity(i));
		if (p_entity == nullptr) continue;
		if (p_entity == local) continue;
		if (!p_entity->IsAlive()) continue;
		if (p_entity->GetTeam() == local->GetTeam()) continue;
		if (p_entity->IsDormant()) continue;
		if (!p_entity->IsPlayer()) continue;

		auto found = false;
		auto points_copy = points; // copy data so that we compair it to the original later to find the lowest thickness
		auto enemy_eyes = p_entity->GetEyePos();

		for (auto &z : points_copy) // now we get the thickness for all of the data
		{
			const Vector tmp(10, z.angle, 0.0f);
			Vector head;
			Math::AngleVectors(tmp, &head);
			head *= ((16.0f + 3.0f) + ((16.0f + 3.0f) * sin(DEG2RAD(10.0f)))) + 7.0f;
			head += local_position;
			const auto local_thickness = get_thickness(head, enemy_eyes);//i really need my source for this bit, i forgot how it works entirely Autowall::GetThickness1(head, hacks.m_local_player, p_entity);
			z.thickness = local_thickness;

			if (local_thickness != 0) // if theres a thickness of 0 dont use this data
			{ 
				found = true;
			}
		}

		if (!found) // dont use
			continue;

		for (auto z = 0; points_copy.size() > z; z++)
			if (points_copy[z].thickness < points[z].thickness || points[z].thickness == -1) // find the lowest thickness so that we can hide our head best for all entities
				points[z].thickness = points_copy[z].thickness;

	}
	float best = 0;
	for (auto &i : points)
		if ((i.thickness > best || i.thickness == -1) && i.thickness != 0) // find the best hiding spot (highest thickness)
		{
			best = i.thickness;
			cmd->viewangles.y = i.angle;
			//auto ret = true;
		}
}
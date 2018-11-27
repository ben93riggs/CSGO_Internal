#pragma once
#include "Singleton.h"

class Vector;
class C_CSPlayer;

class AALinesEnemy : public singleton<AALinesEnemy>
{
public:
	void PaintTraverse(C_CSPlayer* pLocal) const;
};

class AALines : public singleton<AALines>
{
	void rotate_point(Vector& point_out, Vector& c, float angle, float dist_from_center) const;
	static void Normalize360(float& angle);
	void ScopedAALines(C_CSPlayer* pLocal) const;
public:
	void PaintTraverse(C_CSPlayer* pLocal) const;
};
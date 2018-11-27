#pragma once
#include "Singleton.h"

class Color;
class Vector;

class BulletBeams : public singleton<BulletBeams>
{
public:
	static void DrawBeam(Vector src, Vector dst, Color col);
};
#pragma once
#include <vector>
#include "Singleton.h"
#include "SourceEngine/Vector.hpp"
#include "SourceEngine/Color.hpp"


class C_CSPlayer;

class CTimer
{
	__int64 initialTS, currentTS;
	float secsPerCount;

public:
	CTimer()
	{
		__int64 countsPerSec = initialTS = currentTS = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		if (countsPerSec == 0)
			secsPerCount = 1.0f;
		else
			secsPerCount = 1.0f / (float)countsPerSec;
	}
	void init()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&initialTS);
	}
	float diff()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTS);
		return ((float)currentTS - (float)initialTS)*secsPerCount;
	}
};

struct CTraceObj
{
	CTraceObj() = default;
	CTraceObj(Color, unsigned int);

	Color m_colorTrace;
	unsigned int m_iMaxLength;
	std::vector<Vector> m_vecTracePoints;
	CTimer m_timerPointLock;
};

class NadeTracer : public singleton<NadeTracer>
{
public:
	static void AddTracer(C_CSPlayer*, Color, unsigned int);
	static void Draw();
	static void Clear();
};

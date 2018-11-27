#include "NadeTracer.h"
#include <set>
#include "CRender.h"
#include "C_CSPlayer.h"
#include "Globals.h"
#include "Math.hpp"

std::unordered_map<C_CSPlayer*, CTraceObj> m_mapGrenades;
std::set<C_CSPlayer*> m_setGrenadeExists;

void NadeTracer::AddTracer(C_CSPlayer* pGrenade, Color colorTrace, unsigned int iMaxLength)
{
	m_setGrenadeExists.insert(pGrenade);
	if (m_mapGrenades.find(pGrenade) == m_mapGrenades.end())
		m_mapGrenades[pGrenade] = CTraceObj(colorTrace, iMaxLength);

	if (m_mapGrenades[pGrenade].m_timerPointLock.diff() > 0.025f) //25 ms
	{
		m_mapGrenades[pGrenade].m_vecTracePoints.push_back(pGrenade->GetOrigin());
		if (m_mapGrenades[pGrenade].m_vecTracePoints.size() > m_mapGrenades[pGrenade].m_iMaxLength)
			m_mapGrenades[pGrenade].m_vecTracePoints.erase(m_mapGrenades[pGrenade].m_vecTracePoints.begin());

		m_mapGrenades[pGrenade].m_timerPointLock.init();
	}
}

void NadeTracer::Draw()
{
	Vector vecLastScreenPos;
	bool bInit = false;

	for (auto& traceObj : m_mapGrenades)
	{
		for (auto& vecPos : traceObj.second.m_vecTracePoints)
		{
			Vector vecScreenPos;
			if (Math::WorldToScreen(vecPos, vecScreenPos))
			{
				if (bInit){
					CRender::get().DrawLine(vecLastScreenPos.x, vecLastScreenPos.y, vecScreenPos.x, vecScreenPos.y, traceObj.second.m_colorTrace);
					CRender::get().DrawLine(vecLastScreenPos.x-1, vecLastScreenPos.y-1, vecScreenPos.x-1, vecScreenPos.y-1, traceObj.second.m_colorTrace);
				}

				vecLastScreenPos = vecScreenPos;
				bInit = true;
			}
		}
		bInit = false;
	}
}

void NadeTracer::Clear()
{
	for (auto it = m_mapGrenades.begin(); it != m_mapGrenades.end(); ++it)
	{
		if (m_setGrenadeExists.find((*it).first) == m_setGrenadeExists.end())
		{
			it = m_mapGrenades.erase(it);
			if (it == m_mapGrenades.end())
				break;
		}
	}

	m_setGrenadeExists.clear();
}

CTraceObj::CTraceObj(Color colorTrace, unsigned int iMaxLength)
{
	m_colorTrace = colorTrace;
	m_iMaxLength = iMaxLength;
	m_timerPointLock.init();
}
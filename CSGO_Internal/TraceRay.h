#pragma once
#include "SourceEngine/SDK.hpp"

typedef void(__thiscall* TraceRay_t)(IEngineTrace*, Ray_t&, unsigned int, CTraceFilter*, trace_t&);
TraceRay_t g_fnOriginalTraceRay;

inline void __fastcall hooks::trace_ray(IEngineTrace* ths, void* ecx, Ray_t& ray, unsigned int mask, CTraceFilter* filter, trace_t& trace)
{
	g_fnOriginalTraceRay(ths, ray, mask, filter, trace);

	if (!MenuVars::get().g_bTraceRay)
		return;

	if (g::using_bullet_handler) 
		trace.surface.flags |= SURF_SKY;
}
#pragma once
#include "Singleton.h"
#include "XorStr.hpp"

class CustomSky : public singleton<CustomSky>
{
	const char* skylist[18] =
	{
		XorStr("cs_baggage_skybox_"),
		XorStr("cs_tibet"),
		XorStr("embassy"),
		XorStr("italy"),
		XorStr("jungle"),
		XorStr("nukeblank"),
		XorStr("office"),
		XorStr("sky_cs15_daylight01_hdr"),
		XorStr("sky_cs15_daylight02_hdr"),
		XorStr("sky_cs15_daylight03_hdr"),
		XorStr("sky_cs15_daylight04_hdr"),
		XorStr("sky_csgo_cloudy01"),
		XorStr("sky_csgo_night02"),
		XorStr("sky_csgo_night02b"),
		XorStr("sky_dust"),
		XorStr("sky_venice"),
		XorStr("vertigo"),
		XorStr("vietnam")
	};

public:
	void Update() const;
};

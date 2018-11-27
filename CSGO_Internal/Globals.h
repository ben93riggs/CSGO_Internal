#pragma once
#include <Windows.h>
#include "SourceEngine/SDK.hpp"
#include "Config.h"
#include "Utils.hpp"

#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / GLOBALS->interval_per_tick))
#define TICKS_TO_TIME(t) (GLOBALS->interval_per_tick * (t))

#define CURRENTVERSION XorStr("1.9.0")

namespace g // Global Stuff
{
	extern bool aimbotting;
	extern bool last_button_state[267];
	extern bool using_bullet_handler;
	extern bool fakelagging;
	extern bool b_in_fake_walk;
	extern char g_sConfigName[32];
	extern ConVar* sv_cheats;
	extern CUserCmd* cmd;
	extern HWND window;
	extern IClientEntity* besttarget;
	extern IDirect3DDevice9* p_device;
	extern IDirect3DTexture9* texture_image;
	extern int choked_packets[64];
	extern int hitmarkerlength;
	extern int weaponconfig;
	extern int currentweaponid;
	extern int i_current_aa;
	extern int current_fov;
	extern int screenx;
	extern int screeny;
	extern int centerx;
	extern int centery;
	extern Player plist[64];
	extern SpoofedConvar* sv_cheats_spoofed;
	extern Vector anti_aim_angles;
	extern Vector anti_aim_fake_angles;
	extern Vector previous_position;
	extern Vector velocity_unpredicted;
	// ReSharper disable once CppInconsistentNaming
	extern VMatrix w2s_matrix;
	extern WNDPROC old_window_proc;
}

class globals : public singleton<globals>
{
public:
	C_CSPlayer* closest_to_crosshair = nullptr;
	bool knifebotting = false;
	bool debugwindow = false;
	bool full_update = false;
};

namespace set_key_code_state
{
	extern bool should_listen;
	extern ButtonCode_t* key_output;
}

template<typename T>
void pop_front(std::vector<T>& vec)
{
	assert(!vec.empty());
	vec.erase(vec.begin());
}
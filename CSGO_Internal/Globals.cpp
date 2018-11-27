#include "Globals.h"
#include <iostream>

namespace g 
{
	HWND window;
	WNDPROC old_window_proc = nullptr; //Old WNDPROC pointer

	IDirect3DDevice9* p_device;
	IDirect3DTexture9* texture_image = nullptr;

	bool b_in_fake_walk = false;
	bool aimbotting = false;
	bool last_button_state[267] = {};

	Vector anti_aim_angles = Vector(0, 0, 0);
	Vector anti_aim_fake_angles = Vector(0,0,0);

	IClientEntity* besttarget = nullptr;

	CUserCmd* cmd = nullptr;

	Player plist[64];

	int choked_packets[64];

	char g_sConfigName[] = "";

	Vector previous_position;

	Vector velocity_unpredicted;

	int hitmarkerlength = 0;

	ConVar* sv_cheats = nullptr;
	SpoofedConvar* sv_cheats_spoofed = nullptr;

	int weaponconfig = 0;
	int currentweaponid = 0;
	int i_current_aa = 0;

	int current_fov = 90.f;

	bool using_bullet_handler = false;

	bool fakelagging = false;

	VMatrix w2s_matrix;

	int screenx;
	int screeny;
	int centerx;
	int centery;
	bool fullupdate = false;
}

namespace set_key_code_state
{
	bool should_listen = false;
	ButtonCode_t* key_output = nullptr;
}
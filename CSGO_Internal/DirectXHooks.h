#pragma once
#include "Images.h"


typedef long(__stdcall* reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
reset_t o_reset;

typedef long(__stdcall* end_scene_t)(IDirect3DDevice9*);
end_scene_t o_end_scene;

typedef long(__stdcall* present_t)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
present_t o_present;

static bool g_bWasInitialized = false;

inline void gui_init(IDirect3DDevice9* p_device)
{
	g::p_device = p_device;

	g_bWasInitialized = true;
}

//menu
inline HRESULT __stdcall hooks::end_scene(IDirect3DDevice9* p_device)
{
	static DWORD dw_allowed_return = 0;

	DWORD dw_return_address;
	__asm
	{
		push eax
		mov eax, [ebp + 4]
		mov dw_return_address, eax
		pop eax
	}

	if (dw_allowed_return != 0 && dw_allowed_return != dw_return_address)
		return o_end_scene(p_device);

	if (!g_bWasInitialized)
		gui_init(p_device);
	if (!ENGINE->IsTakingScreenshot()) //Draw our GUI
	{
		DWORD backup;
		p_device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		p_device->GetRenderState(D3DRS_COLORWRITEENABLE, &backup);
		p_device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
		menu.GUI();
		p_device->SetRenderState(D3DRS_COLORWRITEENABLE, backup);
	}
	else
		p_device->SetRenderState(D3DRS_SRGBWRITEENABLE, true);

	dw_allowed_return = dw_return_address;
	return o_end_scene(p_device);
}

//key input capture
inline LRESULT __fastcall hooks::wnd_proc(void* ecx, void* edx, HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
{
	static auto insert_pressed = false;

	//Captures the keys states
	switch (u_msg) {
	case WM_LBUTTONDOWN:
		if (set_key_code_state::should_listen)
		{
			set_key_code_state::should_listen = false;
			*set_key_code_state::key_output = MOUSE_LEFT;
		}
		break;
	case WM_RBUTTONDOWN:
		if (set_key_code_state::should_listen)
		{
			set_key_code_state::should_listen = false;
			*set_key_code_state::key_output = MOUSE_RIGHT;
		}
		break;
	case WM_RBUTTONUP:
		break;
	case WM_KEYDOWN:
		if (w_param == VK_INSERT)
			insert_pressed = true;

		if (set_key_code_state::should_listen)
		{
			set_key_code_state::should_listen = false;
			*set_key_code_state::key_output = INPUTSYSTEM->VirtualKeyToButtonCode(w_param);
		}
		break;
	case WM_KEYUP:
		if (w_param == VK_INSERT)
			insert_pressed = false;
		break;
	case WM_XBUTTONDOWN:
		if (GET_XBUTTON_WPARAM(w_param) == XBUTTON1)
		{
			if (set_key_code_state::should_listen)
			{
				set_key_code_state::should_listen = false;
				*set_key_code_state::key_output = MOUSE_4;
			}
		}
		else if (GET_XBUTTON_WPARAM(w_param) == XBUTTON2)
		{
			if (set_key_code_state::should_listen)
			{
				set_key_code_state::should_listen = false;
				*set_key_code_state::key_output = MOUSE_5;
			}
		}
		break;
	case WM_MBUTTONDOWN:
		if (set_key_code_state::should_listen)
		{
			set_key_code_state::should_listen = false;
			*set_key_code_state::key_output = MOUSE_MIDDLE;
		}
		break;
	case WM_SYSKEYDOWN:
		if (set_key_code_state::should_listen)
		{
			set_key_code_state::should_listen = false;
			*set_key_code_state::key_output = KEY_LALT;
		}
		break;

	default:
		break;
	}

	//do key binds here
	//if (AAVars::get().aa[G::iCurrentAA].g_bFlipKeyEnabled && !MenuVars::get().g_bMainWindow && ENGINE->IsInGame() && ENGINE->IsConnected() && CVAR->FindVar(XorStr("cl_mouseenable"))->GetBool() && INPUTSYSTEM->IsButtonDown(MenuVars::get().g_kFlipAAKey) != G::lastButtonState[MenuVars::get().g_kFlipAAKey])
	//{
	//	G::lastButtonState[MenuVars::get().g_kFlipAAKey] = !G::lastButtonState[MenuVars::get().g_kFlipAAKey];

	//	if (INPUTSYSTEM->IsButtonDown(MenuVars::get().g_kFlipAAKey))
	//		MenuVars::get().g_bFlipAngles = !MenuVars::get().g_bFlipAngles;
	//}

	for (int i = 0; i < 4; i++)
	{
		if (!MenuVars::get().g_bMainWindow && ENGINE->IsInGame() && ENGINE->IsConnected() && CVAR->FindVar(XorStr("cl_mouseenable"))->GetBool() && INPUTSYSTEM->IsButtonDown(AAVars::get().g_kAAHotKey[i]) != g::last_button_state[AAVars::get().g_kAAHotKey[i]])
		{
			g::last_button_state[AAVars::get().g_kAAHotKey[i]] = !g::last_button_state[AAVars::get().g_kAAHotKey[i]];

			if (INPUTSYSTEM->IsButtonDown(AAVars::get().g_kAAHotKey[i]))
				g::i_current_aa = i;
		}
	}

	//thirdperson
	if (!MenuVars::get().g_bMainWindow && ENGINE->IsInGame() && ENGINE->IsConnected() && CVAR->FindVar(XorStr("cl_mouseenable"))->GetBool() && INPUTSYSTEM->IsButtonDown(MenuVars::get().g_kThirdPersonKey) != g::last_button_state[MenuVars::get().g_kThirdPersonKey])
	{
		g::last_button_state[MenuVars::get().g_kThirdPersonKey] = !g::last_button_state[MenuVars::get().g_kThirdPersonKey];

		if (INPUTSYSTEM->IsButtonDown(MenuVars::get().g_kThirdPersonKey))
			MiscVars::get().g_bThirdPerson = !MiscVars::get().g_bThirdPerson;

		//CLIENTSTATE->ForceFullUpdate();
	}

	//Insert toggles the menu
	{
		//Maybe there is a better way to do this? Not sure
		//We want to toggle when the key is pressed (i.e when it receives a down and then up signal)
		static bool isDown = false;
		static bool isClicked;

		if (insert_pressed) {
			isClicked = false;
			isDown = true;
		}
		else if (!insert_pressed && isDown) {
			isClicked = true;
			isDown = false;
		}
		else {
			isClicked = false;
			isDown = false;
		}

		if (isClicked) {
			MenuVars::get().g_bMainWindow = !MenuVars::get().g_bMainWindow;

			//Use cl_mouseenable convar to disable the mouse when the window is open 
			CVAR->FindVar(XorStr("cl_mouseenable"))->SetValue(static_cast<bool>(!MenuVars::get().g_bMainWindow));
		}
	}

	if (g_bWasInitialized && MenuVars::get().g_bMainWindow /*&& consumed*/)
		return true; //Input was consumed, return

					 //Input was not consumed by the GUI, call original WindowProc to pass the input to the game
	return CallWindowProc(g::old_window_proc, h_wnd, u_msg, w_param, l_param);
}

//d3d9 reset hook (see comments for explanation)
inline HRESULT __stdcall hooks::reset(IDirect3DDevice9* p_device, D3DPRESENT_PARAMETERS* p_presentation_parameters)
{
	//Correctly handling Reset calls is very important if you have a DirectX hook.
	//IDirect3DDevice9::Reset is called when you minimize the game, Alt-Tab or change resolutions.
	//When it is called, the IDirect3DDevice9 is placed on "lost" state and many related resources are released
	//This means that we need to recreate our own resources when that happens. If we dont, we crash.

	//GUI wasnt initialized yet, just call Reset and return
	if (!g_bWasInitialized)
	{
		return o_reset(p_device, p_presentation_parameters);
	}

	//Device is on LOST state.

	//Invalidate GUI resources
	if (g::texture_image)
		g::texture_image->Release();
	//InvalidateDeviceObjects();

	//Call original Reset.
	const auto hr = o_reset(p_device, p_presentation_parameters);

	//Recreate GUI resources
	D3DXCreateTextureFromFileInMemoryEx(g::p_device, cs_png, cs_png_size, 248, 30, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, &g::texture_image);
	//CreateDeviceObjects();

	return hr;
}

inline HRESULT __stdcall hooks::present(IDirect3DDevice9* p_device, const RECT *p_source_rect, const RECT *p_dest_rect, HWND h_dest_window_override, const RGNDATA *p_dirty_region)
{
	/*if (!G::g_bWasInitialized)
	GUI_Init(pDevice);
	else*/
	//if (!ENGINE->IsTakingScreenshot()) //Draw our GUI
	//	menu.GUI();

	return o_present(p_device, p_source_rect, p_dest_rect, h_dest_window_override, p_dirty_region);
}
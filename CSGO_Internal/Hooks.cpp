#include "Hooks.hpp"
#include "VFTableHook.hpp"

#include "Globals.h"
#include "Utils.hpp"

#include <intrin.h>

#include "XorStr.hpp"
#include "DrawManager.hpp"
#include "Config.h"
#include "NetvarHooks.h"
#include "SpoofedConvar.hpp"

#include "Hooks/CreateMove.h"
#include "Hooks/PaintTraverse.h"
#include "Hooks/FrameStageNotify.h"
#include "Hooks/DrawModelExecute.h"
#include "Hooks/OverrideView.h"
#include "Hooks/PlaySound.h"
#include "Hooks/GetViewModelFOV.h"
#include "Hooks/EmitSound.h"
#include "Hooks/FireEventClientSide.h"
#include "Hooks/FireBullets.h"
#include "InPrediction.h"
#include "Hooks/DoPostScreenSpaceEffects.h"
#include "Hooks/SceneEnd.h"
#include "Hooks/TempEntities.h"
#include "SetupBones.h"
#include "TraceRay.h"

#include "Protections/NoDebug.h"

#include <windows.h>
#include <cstdio>
#include <functional>
#include "GUI.h"
#include "EventListeners.h"
#include "OnScreenSizeChanged.h"

#include "Auth.h"
#include <shlobj.h>
#include <d3dx9.h>
#include "DirectXHooks.h"
#include "Hooks/IsPlayingTimeDemo.h"
#include "vmthook.h"

namespace hooks
{

	#pragma region VFT Hook Variables
	std::vector<std::shared_ptr<VFTableHook>> vft_hooks;
	std::vector<vmthook> vmt_hooks;
	vmthook	hlclient_hook;

	// ReSharper disable once CppInconsistentNaming
	std::shared_ptr<VFTableHook>			p_d3d_device9_hook;
	std::shared_ptr<VFTableHook>			p_client_mode_hook;
	std::shared_ptr<VFTableHook>			p_mat_surface_hook;
	std::shared_ptr<VFTableHook>			p_panel_hook;
	std::shared_ptr<VFTableHook>			p_model_render;
	std::shared_ptr<VFTableHook>			p_in_prediction;
	std::shared_ptr<VFTableHook>			p_game_event_manager;
	std::shared_ptr<VFTableHook>			p_fire_bullets;
	std::shared_ptr<VFTableHook>			p_trace_ray;
	std::shared_ptr<VFTableHook>			p_engine_sound;
	std::shared_ptr<VFTableHook>			p_engine_client;
	std::shared_ptr<VFTableHook>			p_input_internal;
	std::shared_ptr<VFTableHook>			p_render_view;
	std::shared_ptr<VFTableHook>			p_engine_trace;
	std::shared_ptr<VFTableHook>			p_client_state_temp_entities;

	
	//std::unique_ptr<DrawManager>			p_renderer;

	template<typename T>
	void hook_vft(std::shared_ptr<VFTableHook>& hook, T pointer)
	{
		hook = std::make_shared<VFTableHook>(reinterpret_cast<PPDWORD>(pointer), true);
		vft_hooks.push_back(hook);
	}

#pragma endregion

	void restore(const RESTORE_MESSAGES error)
	{		
		NetvarManager::UnhookRecvProxies();

		for (auto listener : listeners)
			delete listener;

		for (auto& hook : vft_hooks)
			hook->RestoreTable();

		if (g::texture_image)
			g::texture_image->Release();
		
		//DestroyContext();
		PostError(error);

		//Restore the WindowProc
		SetWindowLongPtr(g::window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g::old_window_proc));

		PostMessageW(GetConsoleWindow(), WM_CLOSE, 0, 0);
		FreeConsole();
	}

	std::string get_app_data_path()
	{
		TCHAR sz_path[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, sz_path)))
		{
			char buffer[MAX_PATH];
			wcstombs(buffer, sz_path, MAX_PATH);
			return buffer;
		}

		MessageBox(nullptr, L"Failed to get AppData path!", L"Error", 0);
		return "";
	}

	void initialize()
	{
		Beep(0x200, 0x100);

		try
		{
			AllocConsole();
			freopen("CON", "w", stdout);
			system("color b");

			auto path = get_app_data_path() + XorStr("\\CSGO_Internal\\");
			CreateDirectoryA(path.c_str(), nullptr);

			authorize();

			while (!((g::window = FindWindowA(XorStr("Valve001"), nullptr)))) Sleep(200);

			NetvarManager::Instance()->CreateDatabase();
			#ifdef DEV_ENABLED
			NetvarManager::Instance()->Dump(XorStr("netvar_dump.txt"));
			#endif

			//CreateContext();

			//Replace the WindowProc with our own to capture user input
			g::old_window_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g::window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc)));

			//safe vmt hooks
			hlclient_hook.setup(CLIENT, Utils::get_client_dll().c_str());
			o_frame_stage_notify = hlclient_hook.hook_index<frame_stage_notify_t>(37, reinterpret_cast<frame_stage_notify_t>(frame_stage_notify));


			//normal vmt hooks
			hook_vft(p_d3d_device9_hook, **reinterpret_cast<uint32_t**>(Utils::FindSignature(XorStr("shaderapidx9.dll"), XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1));
			hook_vft(p_client_mode_hook, CLIENTMODE);
			hook_vft(p_mat_surface_hook, MATSURFACE);
			hook_vft(p_panel_hook, VGUIPANEL);
			hook_vft(p_model_render, MODELRENDER);
			hook_vft(p_engine_sound, ENGINESOUND);
			hook_vft(p_game_event_manager, GAMEEVENTMANAGER);
			//HookVFT(pInputInternal, INPUTINTERNAL);
			hook_vft(p_fire_bullets, FIREBULLETS);
			//HookVFT(pInPrediction, PREDICTION);
			hook_vft(p_render_view, RENDERVIEW);
			hook_vft(p_engine_trace, ENGINETRACE);
			hook_vft(p_engine_client, ENGINE);

			SETUPCONFIG()
			
			o_reset = p_d3d_device9_hook->Hook(16, static_cast<reset_t>(reset));
			o_end_scene = p_d3d_device9_hook->Hook(42, static_cast<end_scene_t>(end_scene));

			o_create_move = p_client_mode_hook->Hook(24, reinterpret_cast<create_move_t>(create_move)); //232
			g_fnOriginalOverrideView = p_client_mode_hook->Hook(18, reinterpret_cast<OverrideView_t>(override_view));
			g_fnOriginalGetViewModelFOV = p_client_mode_hook->Hook(35, reinterpret_cast<GetViewModelFOV_t>(get_view_model_fov));
			g_fnOriginalPlaySound = p_mat_surface_hook->Hook(82, reinterpret_cast<PlaySound_t>(play_sound));
			o_paint_traverse = p_panel_hook->Hook(41, reinterpret_cast<paint_traverse_t>(paint_traverse));
			g_fnOriginalDrawModelExecute = p_model_render->Hook(21, reinterpret_cast<DrawModelExecute_t>(draw_model_execute)); //275
			g_fnOriginalEmitSound = p_engine_sound->Hook(5, reinterpret_cast<EmitSoundAtt_t>(Hooked_EmitSound));
			o_fire_event_client_side = p_game_event_manager->Hook(9, reinterpret_cast<fire_event_client_side_t>(fire_event_client_side)); //237
			o_do_post_screen_space_effects = p_client_mode_hook->Hook(44, reinterpret_cast<do_post_screen_space_effects_t>(do_post_screen_space_effects));
			g_fnOriginalTraceRay = p_engine_trace->Hook(5, reinterpret_cast<TraceRay_t>(trace_ray));
			o_scene_end = p_render_view->Hook(9, reinterpret_cast<scene_end_t>(scene_end));
			g_fnOriginalOnScreenSizeChanged = p_mat_surface_hook->Hook(116, reinterpret_cast<OnScreenSizeChanged_t>(on_screen_size_changed));
			g_fnOriginalIsPlayingTimeDemo = p_engine_client->Hook(84, reinterpret_cast<IsPlayingTimeDemo_t>(is_playing_time_demo));
			//g_fnOriginalFireBullets_PostDataUpdate = pFireBullets->Hook(7, reinterpret_cast<TE_FireBullets_PostDataUpdateFn>(TE_FireBullets_PostDataUpdate));
			//g_fnOriginalTempEntities = VFTableHook::HookManual<TempEntities_t>(*reinterpret_cast<uintptr_t**>(reinterpret_cast<int>(Interface->ClientState()) + 0x8), 36, reinterpret_cast<TempEntities_t>(Hooked_TempEntities));
			//g_fnOriginalSetupBones = VFTableHook::HookManual<SetupBones_t>(reinterpret_cast<uintptr_t*>(Interface->CCSPlayerRenderableVTable()), 13, reinterpret_cast<SetupBones_t>(Hooked_SetupBones));
			//g_fnOriginalRunCommand = pInPrediction->Hook(19, reinterpret_cast<RunCommand_t>(Hooked_RunCommand));

			//o_present = pD3DDevice9Hook->Hook(17, static_cast<Present_t>(Hooked_Present));
			//g_fnOriginalSetKeyCodeState = g_pInputInternal->Hook(91, reinterpret_cast<SetKeyCodeState_t>(Hooked_SetKeyCodeState));
			//g_fnOriginalSetMouseCodeState = g_pInputInternal->Hook(92, reinterpret_cast<SetMouseCodeState_t>(Hooked_SetMouseCodeState));
			//g_fnOriginalInPrediction = pInPrediction->Hook(14, reinterpret_cast<InPrediction_t>(Hooked_InPrediction));

			g_pPlayerHurtListener = new player_hurt_listener;
			g_pBulletImpactListener = new bullet_impact_listener;
			g_pItemPurchaseListener = new item_purchase_listener;

			HookRecvProxies();

			#ifndef DEV_ENABLED
			VIRTUALIZER_FISH_WHITE_END
			#endif

			DevMsg::get().OutputMessage(Color::MediumPurple(), std::string(XorStr("initialization successful")).c_str());
#ifdef RELEASE_VERSION
			DevMsg::get().OutputMessage(Color::MediumPurple(), std::string(XorStr("version: ") + std::string(CURRENTVERSION)).c_str());
#else
			DevMsg::get().OutputMessage(Color::MediumPurple(), std::string(XorStr("version: beta")).c_str());
#endif
			
			Beep(0x300, 0x100);
		}
		catch (...)
		{
			std::cout << XorStr("Error Initializing...") << std::endl;
		}
	}

}

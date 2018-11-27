#pragma once
#include <Windows.h>
#include <d3d9.h>
#define mb(text) MessageBoxA(NULL, text, NULL, NULL);
#include "SourceEngine/SDK.hpp"

//Link with the D3D9 implementations
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class C_CSPlayer;

namespace hooks
{
	enum RESTORE_MESSAGES
	{
		OK,
		AUTH_FAILURE,
		HTTP_FAILURE,
		DEBUG1,
		DEBUG2,
		DEBUG3,
		DEBUG4,
		DEBUG5,
		DEBUGGER_OLLY,
		DEBUGGER_WINDBG,
		OBJECTLISTCHECK,
		CHECKHARDWAREBREAKPOINTS,
		CHECKPROCESSDEBUGFLAGS,
		DEBUGOBJECTCHECK,
		CANOPENCSRSS,
		CHECKGLOBALFLAGS,
		VALIDHDDSIZE,
		SWALLOW_REDPILL,
		ISINSIDEVPC,
		ISINSIDEVMWARE,
		MEMORYBREAKPOINTDEBUGGERCHECK,
		PROCESS_DETACH



		//INT2DCHECK,
		//CHECKOUTPUTDEBUGSTRING,
		//ISDBGPRESENTPREFIXCHECK,


	};

	void initialize();
	void restore(RESTORE_MESSAGES error = OK);

	//---------------------------------------------------
	// Hook prototypes
	//---------------------------------------------------

	//---------------------------------------------------
	// Hooked functions
	//---------------------------------------------------
	HRESULT		__stdcall end_scene(IDirect3DDevice9* pDevice);
	HRESULT		__stdcall reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	HRESULT		__stdcall present(IDirect3DDevice9* pDevice, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);

	void		__fastcall play_sound(void* ecx, void* edx, const char* filename);
	void		__fastcall override_view(IClientMode* ecx, void* edx, CViewSetup* vsView);
	void		__fastcall paint_traverse(IPanel* ecx, void* edx, int VGUIPanel, bool ForceRepaint, bool AllowForce);
	bool		__fastcall create_move(IClientMode* ecx, void* edx, float sample_input_frametime, CUserCmd* pCmd);
	void		__fastcall draw_model_execute(IVModelRender* ecx, void* edx, void* context, void* state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld);
	void		__fastcall run_command(void* thisptr, void* edx, C_CSPlayer* player, CUserCmd* pCmd, IMoveHelper* moveHelper);
	bool		__fastcall fire_event_client_side(IGameEventManager2* ecx, void* edx, IGameEvent* gEvent);
	void		__fastcall frame_stage_notify(void* ecx, void* edx, ClientFrameStage_t stage);
	
	bool		__fastcall temp_entities(void* ecx, void* edx, void* msg);
	bool		__fastcall setup_bones(IClientRenderable* ecx, void* edx, matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);

	float		__fastcall get_view_model_fov(void* ecx, void* edx);

	bool		__fastcall is_playing_time_demo(IVEngineClient* ecx, void* edx);

	int			__fastcall do_post_screen_space_effects(IClientMode* ecx, void* edx, int a1);

	void		__fastcall scene_end(IVRenderView* ecx, void* edx);

	void		__fastcall set_key_code_state(IInputInternal* ecx, void* edx, ButtonCode_t code, bool bPressed);
	void		__fastcall set_mouse_code_state(IInputInternal* ecx, void* edx, ButtonCode_t code, MouseCodeState_t state);

	void		__fastcall trace_ray(IEngineTrace* ths, void* ecx, Ray_t& ray, unsigned int mask, CTraceFilter* filter, trace_t& trace);

	bool		__fastcall in_prediction(void* ecx, void* edx);
	bool		__fastcall is_playing_time_demo(IVEngineClient* ecx, void* edx);

	//void		__stdcall FireBullets_PostDataUpdate(C_TEFireBullets* ecx, /*void* edx, */DataUpdateType_t updateType);
	//__declspec (naked) void		__stdcall TE_FireBullets_PostDataUpdate(C_TEFireBullets* ecx, /*void* edx, */DataUpdateType_t updateType);

	int			__fastcall send_datagram(INetChannel * ecx, void * edx, void * datagram);

	void		__fastcall on_screen_size_changed(ISurface* ecx, void* edx, int oldWidth, int oldHeight);

	//We're gonna replace the main window's WNDPROC for this one to capture user input and pass it down to the GUI
	LRESULT		__fastcall wnd_proc(void* ecx, void* edx, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}
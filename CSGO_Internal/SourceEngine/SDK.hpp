#pragma once
#include "Definitions.hpp"
#include "CRC.hpp"
#include "Vector.hpp"
#include "Vector2D.hpp"
#include "Vector4D.hpp"
#include "QAngle.hpp"
#include "CHandle.hpp"
#include "CGlobalVarsBase.hpp"
#include "ClientClass.hpp"
#include "Color.hpp"
#include "IBaseClientDll.hpp"
#include "IClientEntityList.hpp"
#include "IClientNetworkable.hpp"
#include "IClientRenderable.hpp"
#include "IClientThinkable.hpp"
#include "IClientUnknown.hpp"
#include "IPanel.hpp"
#include "ISurface.hpp"
//#include "IMaterial.h"
#include "IVEngineClient.hpp"
#include "IEngineTrace.hpp"
#include "PlayerInfo.hpp"
#include "Recv.hpp"
#include "VMatrix.hpp"
#include "IClientMode.hpp"
#include "CInput.hpp"
#include "ICvar.hpp"
#include "Convar.h"
#include "../SpoofedConvar.hpp"
#include "IPhysicsSurfaceProps.h"
#include "IPrediction.h"
#include "IVModelRender.h"
#include "IVModelInfo.h"
#include "IVRenderView.h"
#include "IGameEvents.h"
#include "CTEFireBullets.h"
#include "IInputSystem.h"
#include "IEngineSound.h"
#include "IClientState.h"
#include "../memalloc.h"
#include "IVDebugOverlay.h"
#include "ILocalize.h"
#include "ISteamHTTP.h"
#include "CSGameRulesProxy.h"
#include "../CSPlayerResource.h"
#include "../C_AnimationLayer.h"
#include "../datamap.h"
#include "../IViewRenderBeams.h"
#include "VarMap.h"
#include "../IWeaponSystem.h"

#pragma warning(disable : 4305)
#pragma warning(disable : 4244)

#define ENGINE Interface->Engine()
#define CLIENT Interface->Client()
#define ENTITYLIST Interface->EntityList()
#define GLOBALS Interface->Globals()
#define VGUIPANEL Interface->VGUIPanel()
#define MATSURFACE Interface->MatSurface()
#define INPUT Interface->Input()
#define ENGINETRACE Interface->EngineTrace()
#define CVAR Interface->CVar()
#define CLIENTMODE Interface->ClientMode()
#define PHYSPROPS Interface->PhysProps()
#define PREDICTION Interface->Prediction()
#define MODELRENDER Interface->ModelRender()
#define MODELINFO Interface->ModelInfo()
#define MATSYSTEM Interface->MaterialSystem()
#define RENDERVIEW Interface->RenderView()
#define GAMEMOVEMENT Interface->GameMovement()
#define GAMEEVENTMANAGER Interface->GameEventManager()
#define FIREBULLETS Interface->FireBullets()
#define INPUTSYSTEM Interface->InputSystem()
#define ENGINESOUND Interface->EngineSound()
#define CLIENTSTATE Interface->ClientState()
#define DEBUGOVERLAY Interface->DebugOverlay()
#define LOCALIZE Interface->Localize()
#define STEAMHTTP Interface->SteamHTTP()
#define INPUTINTERNAL Interface->InputInternal()
#define MEMALLOC Interface->MemAlloc()
#define WEAPONSYSTEM Interface->WeaponSystem()
#define VIEWRENDERBEAM Interface->ViewRenderBeam()
#define PLAYERRESOURCE Interface->PlayerResource()

class Interfaces
{
	IVEngineClient*						m_pEngine = nullptr;
	IBaseClientDLL*						m_pClient = nullptr;
	IClientEntityList*					m_pEntityList = nullptr;
	CGlobalVarsBase*					m_pGlobals = nullptr;
	IPanel*								m_pVGuiPanel = nullptr;
	ISurface*							m_pVGuiSurface = nullptr;
	CInput*								m_pInput = nullptr;
	IEngineTrace*						m_pEngineTrace = nullptr;
	ICvar*								m_pCVar = nullptr;
	IClientMode*						m_pClientMode = nullptr;
	IPhysicsSurfaceProps*				m_Physprops = nullptr;
	IPrediction*						m_pPrediction = nullptr;
	IVModelRender*						m_pModelRender = nullptr;
	IVModelInfo*						m_pModelInfo = nullptr;
	IMaterialSystem*					m_pMaterialSystem = nullptr;
	IVRenderView*						m_pRenderView = nullptr;
	IGameMovement*						m_pGameMovement = nullptr;
	IGameEventManager2*					m_pGameEventManager = nullptr;
	C_TEFireBullets*					m_pFireBullets = nullptr;
	IInputSystem*						m_pInputSystem = nullptr;
	IEngineSound*						m_pEngineSound = nullptr;
	IClientState*						m_pClientState = nullptr;
	IVDebugOverlay*						m_pDebugOverlay = nullptr;
	ILocalize*							m_pLocalize = nullptr;
	ISteamHTTP*							m_pSteamHTTP = nullptr;
	CSGameRulesProxy*					m_pGameRulesProxy = nullptr;
	IInputInternal*						m_pInputInternal = nullptr;
	CSPlayerResource*					m_pPlayerResource = nullptr;
	IMoveHelper*						m_pMoveHelper = nullptr;
	IMemAlloc*							m_pMemAlloc = nullptr;
	IViewRenderBeams*					g_pViewRenderBeams = nullptr;
	IWeaponSystem*						m_pWeaponSystem = nullptr;

	DWORD								m_dwInvalidateBoneCache = NULL;
	DWORD								m_dwSequenceActivity = NULL;
	DWORD								m_dwAnimOverlay = NULL;
	DWORD								m_dwInvalidatePhysicsRecursive = NULL;
	DWORD								m_dwCCSPlayerRenderableVTable = NULL; 
	DWORD								m_dwUpdateAnimState = NULL;

public:
	Interfaces() {}

	//non-interfaces:
	DWORD								InvalidateBoneCache();
	DWORD								SequenceActivity();
	DWORD								AnimOverlay();
	DWORD								InvalidatePhysicsRecursive();
	DWORD								CCSPlayerRenderableVTable();
	DWORD UpdateAnimState();

	//other (sig based) interfaces:
	CSGameRulesProxy*					GameRulesProxy();
	CSPlayerResource*					PlayerResource();
	IMoveHelper*						MoveHelper();

	//main interfaces:
	IVEngineClient*						Engine();
	IBaseClientDLL*						Client();
	IClientEntityList*					EntityList();
	CGlobalVarsBase*					Globals();
	IPanel*								VGUIPanel();
	ISurface*							MatSurface();
	CInput*								Input();
	IEngineTrace*						EngineTrace();
	ICvar*								CVar();
	IClientMode*						ClientMode();
	IPhysicsSurfaceProps*				PhysProps();
	IPrediction*						Prediction();
	IVModelRender*						ModelRender();
	IVModelInfo*						ModelInfo();
	IMaterialSystem*					MaterialSystem();
	IVRenderView*						RenderView();
	IGameMovement*						GameMovement();
	IGameEventManager2*					GameEventManager();
	C_TEFireBullets*					FireBullets();
	IInputSystem*						InputSystem();
	IEngineSound*						EngineSound();
	IClientState*						ClientState();
	IVDebugOverlay*						DebugOverlay();
	ILocalize*							Localize();
	ISteamHTTP*							SteamHTTP();
	IInputInternal*						InputInternal();
	IMemAlloc*							MemAlloc();
	IWeaponSystem*						WeaponSystem();
	IViewRenderBeams*					ViewRenderBeam();
};

extern Interfaces* Interface;
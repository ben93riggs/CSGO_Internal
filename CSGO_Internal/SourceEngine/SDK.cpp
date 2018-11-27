#include "SDK.hpp"

#include "../Utils.hpp"
#include "../XorStr.hpp"
#include "../IWeaponSystem.h"

CreateInterfaceFn GetFactory(HMODULE hMod)
{
	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(hMod, XorStr("CreateInterface")));
}

template<typename T>
T* CaptureInterface(CreateInterfaceFn f, const char* szInterfaceVersion)
{
	T* temp = (T*)f(szInterfaceVersion, nullptr);

	while (!temp)
		temp = (T*)f(szInterfaceVersion, nullptr);

	return temp;
}

HMODULE WaitGetModuleHandle(std::string module_name)
{
	HMODULE ModuleHandle = nullptr;
	while (!ModuleHandle)
	{
		ModuleHandle = GetModuleHandleA(module_name.c_str());
		if (!ModuleHandle)
			Sleep(50);
	}

	return ModuleHandle;
}

CSGameRulesProxy* Interfaces::GameRulesProxy()
{
	if (!m_pGameRulesProxy)
		m_pGameRulesProxy = *(*reinterpret_cast<CSGameRulesProxy***>(Utils::FindSignature(Utils::get_client_dll(), XorStr("A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 0F 84 ? ? ? ? 0F 10 05")) + 0x1));

	return m_pGameRulesProxy;
}
CSPlayerResource* Interfaces::PlayerResource()
{
	if (!m_pPlayerResource)
		m_pPlayerResource = **(CSPlayerResource***)(Utils::FindSignature(Utils::get_client_dll(), XorStr("8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7")) + 2);

	return m_pPlayerResource;
}
IMoveHelper* Interfaces::MoveHelper()
{
	if (!m_pMoveHelper)
		m_pMoveHelper = **reinterpret_cast<IMoveHelper***>(Utils::FindPattern(Utils::get_client_dll(), reinterpret_cast<PBYTE>(XorStr("\x8B\x0D\x00\x00\x00\x00\x8B\x45\x00\x51\x8B\xD4\x89\x02\x8B\x01")), XorStr("xx????xx?xxxxxxx")) + 2);

	return m_pMoveHelper;
}
DWORD Interfaces::InvalidateBoneCache()
{
	while (!m_dwInvalidateBoneCache)
		m_dwInvalidateBoneCache = Utils::FindPattern(Utils::get_client_dll(), XorStr("FF 90 ? ? ? ? 80 3D ? ? ? ? ? 74 17"));

	return m_dwInvalidateBoneCache;
}
DWORD Interfaces::AnimOverlay()
{
	while (!m_dwAnimOverlay)
		m_dwAnimOverlay = *(DWORD*)(Utils::FindSignature(Utils::get_client_dll(), XorStr("8B 9F ?? ?? ?? ?? 8D 43 14 39 30 74 21")) + 0x2); //"55 8B EC 51 53 8B 5D 08 33 C0" OR //55 8B EC 57 8B F9 8B 97 ? ? ? ? 85 D2

	return m_dwAnimOverlay;
}

DWORD Interfaces::InvalidatePhysicsRecursive()
{
	while (!m_dwInvalidatePhysicsRecursive)
		m_dwInvalidatePhysicsRecursive = Utils::FindSignature(Utils::get_client_dll(), XorStr("55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56 83 E0 04"));

	return m_dwInvalidatePhysicsRecursive;
}

DWORD Interfaces::CCSPlayerRenderableVTable()
{
	while (!m_dwCCSPlayerRenderableVTable)
		m_dwCCSPlayerRenderableVTable = *(DWORD*)(Utils::FindSignature(Utils::get_client_dll(), XorStr("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C")) + 0x4E);

	return m_dwCCSPlayerRenderableVTable;
}

DWORD Interfaces::UpdateAnimState()
{
	while (!m_dwUpdateAnimState)
		m_dwUpdateAnimState = *(DWORD*)(Utils::FindSignature(Utils::get_client_dll(), XorStr("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24")));

	return m_dwUpdateAnimState;
}

DWORD Interfaces::SequenceActivity()
{
	while (!m_dwSequenceActivity)
		m_dwSequenceActivity = Utils::FindPattern(Utils::get_client_dll(), XorStr("55 8B EC 83 7D 08 FF 56 8B F1 74 3D"));

	return m_dwSequenceActivity;
}

IVEngineClient* Interfaces::Engine()
{
	while (!m_pEngine)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("engine.dll")));
		m_pEngine = CaptureInterface<IVEngineClient>(pfnFactory, XorStr("VEngineClient014"));
		assert(m_pEngine);
	}
	return m_pEngine;
}
IBaseClientDLL* Interfaces::Client()
{
	while (!m_pClient)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(Utils::get_client_dll()));
		m_pClient = CaptureInterface<IBaseClientDLL>(pfnFactory, XorStr("VClient018"));
		assert(m_pClient);
	}
	return m_pClient;
}
IClientEntityList* Interfaces::EntityList()
{
	while (!m_pEntityList)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(Utils::get_client_dll()));
		m_pEntityList = CaptureInterface<IClientEntityList>(pfnFactory, XorStr("VClientEntityList003"));
	}
	return m_pEntityList;
}
CGlobalVarsBase* Interfaces::Globals()
{
	if (!m_pGlobals || m_pGlobals->interval_per_tick == 0)
	{
		m_pGlobals = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<DWORD**>(Client()))[0] + 0x1B);
		assert(m_pGlobals);

	}
	return m_pGlobals;
}
IPanel* Interfaces::VGUIPanel() {
	while (!m_pVGuiPanel) {
		const CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("vgui2.dll")));
		m_pVGuiPanel = CaptureInterface<IPanel>(pfnFactory, XorStr("VGUI_Panel009"));
	}
	return m_pVGuiPanel;
}
ISurface* Interfaces::MatSurface() {
	while (!m_pVGuiSurface) {
		const CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("vguimatsurface.dll")));
		m_pVGuiSurface = CaptureInterface<ISurface>(pfnFactory, XorStr("VGUI_Surface031"));
	}
	return m_pVGuiSurface;
}
CInput* Interfaces::Input()
{
	while (!m_pInput)
	{
		m_pInput = *reinterpret_cast< CInput** >(Utils::FindPattern( Utils::get_client_dll(), "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10" ) + 1);
	}
	return m_pInput;
}
IEngineTrace* Interfaces::EngineTrace()
{
	while (!m_pEngineTrace)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("engine.dll")));
		m_pEngineTrace = CaptureInterface<IEngineTrace>(pfnFactory, XorStr("EngineTraceClient004"));
	}
	return m_pEngineTrace;
}
ICvar* Interfaces::CVar()
{
	while (!m_pCVar)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("vstdlib.dll")));
		m_pCVar = CaptureInterface<ICvar>(pfnFactory, XorStr("VEngineCvar007"));
	}
	return m_pCVar;
}
IClientMode* Interfaces::ClientMode() 
{
	while (!m_pClientMode) 
	{
		auto uAddress = *(DWORD*)(Utils::FindSignature(Utils::get_client_dll(), XorStr("8B 0D ? ? ? ? 8B 01 5D FF")) + 2);
		m_pClientMode = *(IClientMode**)(uAddress);
	}
	return m_pClientMode;
}
IPhysicsSurfaceProps* Interfaces::PhysProps()
{
	while (!m_Physprops)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("vphysics.dll")));
		m_Physprops = CaptureInterface<IPhysicsSurfaceProps>(pfnFactory, XorStr("VPhysicsSurfaceProps001"));
	}
	return m_Physprops;
}
IPrediction* Interfaces::Prediction()
{
	while (!m_pPrediction)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(Utils::get_client_dll()));
		m_pPrediction = CaptureInterface<IPrediction>(pfnFactory, XorStr("VClientPrediction001"));
	}
	return m_pPrediction;
}
IVModelRender* Interfaces::ModelRender()
{
	while (!m_pModelRender)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("engine.dll")));
		m_pModelRender = CaptureInterface<IVModelRender>(pfnFactory, XorStr("VEngineModel016"));
	}
	return m_pModelRender;
}
IVModelInfo* Interfaces::ModelInfo()
{
	while (!m_pModelInfo)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("engine.dll")));
		m_pModelInfo = CaptureInterface<IVModelInfo>(pfnFactory, XorStr("VModelInfoClient004"));
	}
	return m_pModelInfo;
}
IMaterialSystem* Interfaces::MaterialSystem()
{
	while (!m_pMaterialSystem)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("materialsystem.dll")));
		m_pMaterialSystem = CaptureInterface<IMaterialSystem>(pfnFactory, XorStr("VMaterialSystem080"));
	}

	return m_pMaterialSystem;
}
IVRenderView* Interfaces::RenderView()
{
	while (!m_pRenderView)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("engine.dll")));
		m_pRenderView = CaptureInterface<IVRenderView>(pfnFactory, XorStr("VEngineRenderView014"));
	}

	return m_pRenderView;
}
IGameMovement* Interfaces::GameMovement()
{
	while (!m_pGameMovement)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(Utils::get_client_dll()));
		m_pGameMovement = CaptureInterface<IGameMovement>(pfnFactory, XorStr("GameMovement001"));
	}
	return m_pGameMovement;
}
IGameEventManager2* Interfaces::GameEventManager()
{
	while (!m_pGameEventManager)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("engine.dll")));
		m_pGameEventManager = CaptureInterface<IGameEventManager2>(pfnFactory, XorStr("GAMEEVENTSMANAGER002"));
	}
	return m_pGameEventManager;
}
C_TEFireBullets* Interfaces::FireBullets()
{
	while (!m_pFireBullets)
	{
		m_pFireBullets = *(C_TEFireBullets**)(Utils::FindSignature(Utils::get_client_dll(), XorStr("55 8B EC 51 53 56 8B F1 BB ? ? ? ? B8")) + 0x131);
	}
	return m_pFireBullets;
}
IInputSystem* Interfaces::InputSystem()
{
	while (!m_pInputSystem)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("inputsystem.dll")));
		m_pInputSystem = CaptureInterface<IInputSystem>(pfnFactory, XorStr("InputSystemVersion001"));
	}
	return m_pInputSystem;
}
IEngineSound* Interfaces::EngineSound()
{
	while (!m_pEngineSound)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("engine.dll")));
		m_pEngineSound = CaptureInterface<IEngineSound>(pfnFactory, XorStr("IEngineSoundClient003"));
	}
	return m_pEngineSound;
}
IClientState* Interfaces::ClientState()
{
	while (!m_pClientState)
	{
		m_pClientState = **(IClientState***)(Utils::FindSignature(XorStr("engine.dll"), XorStr("8B 3D ?? ?? ?? ?? 8A F9")) + 0x2);
		//m_pClientState = **(IClientState***)(Utils::FindSignature(XorStr("engine.dll"), XorStr("A1 ? ? ? ? 8B 80 ? ? ? ? C3")) + 1);
	}
	return m_pClientState;
}
IVDebugOverlay* Interfaces::DebugOverlay()
{
	while (!m_pDebugOverlay)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("engine.dll")));
		m_pDebugOverlay = CaptureInterface<IVDebugOverlay>(pfnFactory, XorStr("VDebugOverlay004"));
	}

	return m_pDebugOverlay;
}
ILocalize* Interfaces::Localize()
{
	while (!m_pLocalize)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("localize.dll")));
		m_pLocalize = CaptureInterface<ILocalize>(pfnFactory, XorStr("Localize_001"));
	}
	return m_pLocalize;
}
ISteamHTTP* Interfaces::SteamHTTP()
{
	while (!m_pSteamHTTP)
	{
		m_pSteamHTTP = (*reinterpret_cast<SteamContext**>(Utils::FindPattern(Utils::get_client_dll(), (PBYTE)XorStr("\xFF\x15\x00\x00\x00\x00\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x6A\x00"), XorStr("xx????x????x????xx")) + 0x7))->steam_http;
	}

	return m_pSteamHTTP;
}
IInputInternal*	Interfaces::InputInternal()
{
	while (!m_pInputInternal)
	{
		CreateInterfaceFn pfnFactory = GetFactory(WaitGetModuleHandle(XorStr("vgui2.dll")));
		m_pInputInternal = CaptureInterface<IInputInternal>(pfnFactory, XorStr("VGUI_InputInternal001"));
	}

	return m_pInputInternal;
}
IViewRenderBeams* Interfaces::ViewRenderBeam()
{
	if (!g_pViewRenderBeams)
		g_pViewRenderBeams = *(IViewRenderBeams**)(Utils::FindPattern(Utils::get_client_dll(), XorStr("B9 ?? ?? ?? ?? A1 ?? ?? ?? ?? FF 10 A1 ?? ?? ?? ?? B9")) + 1);

	return g_pViewRenderBeams;
}

IMemAlloc* Interfaces::MemAlloc()
{
	while (!m_pMemAlloc)
	{
		m_pMemAlloc = *(IMemAlloc**)GetProcAddress(GetModuleHandleA(XorStr("tier0.dll")), XorStr("g_pMemAlloc"));
	}

	return m_pMemAlloc;
}
IWeaponSystem* Interfaces::WeaponSystem()
{
	while (!m_pWeaponSystem)
	{
		m_pWeaponSystem = *(IWeaponSystem**)(Utils::FindPattern(Utils::get_client_dll(), XorStr("8B 35 ? ? ? ? FF 10 0F B7 C0")) + 0x2);
	}

	return m_pWeaponSystem;
}

Interfaces* Interface = new Interfaces;
#pragma once
#include "Singleton.h"
#include "C_CSPlayer.h"

enum kBoxSettings : int
{
	BOX_OFF,
	BOX_TWO_DIMENSIONAL,
	BOX_THREE_DIMENSIONAL,
	BOX_CORNERS
};

enum kIconSettings : int
{
	ICON_OFF,
	ICON_TEXT,
	ICON_ON
};

enum kFakelagSettings : int
{
	FLAG_OFF,
	FLAG_NORMAL,
	FLAG_ADAPTIVE,
	FLAG_ONPEEK
};

enum kBuyBotSecondarySettings : int
{
	BBS_NONE,
	BBS_DUALIES,
	BBS_DEAGLE_R8
};

enum kBuyBotPrimarySettings : int
{
	BBP_NONE,
	BBP_AUTO,
	BBP_SCOUT,
	BBP_AWP
};

class MenuVars : public singleton<MenuVars>
{
public:
	struct
	{
		bool g_bSelectedConfig[256];
		std::string g_szCurrentConfig = "default";
		char* g_cCurrentConfig = "default";
		char g_cCurrentConfigg[260] = "default";
	} cConfig;

	bool g_bDisableInterp = true;
	bool g_bUseLerp = true;

	bool g_bMainWindow = false;
	bool g_bAATab[4] = { true, false, false, false };
	bool g_bHideTitle = false;
	int g_iCurrentMenuTab = 0;
	int g_iCurrentAimbotMode = 0;
	bool g_bResolveYaw = false;
	bool g_bBackTrackExploit = false;
	bool g_bFakewalk = false;

	ButtonCode_t g_kCircleStrafeKey = KEY_LALT;
	ButtonCode_t g_kThirdPersonKey = KEY_X;
	ButtonCode_t g_kOverrideKey = KEY_LCONTROL;
	ButtonCode_t g_kFakeWalkKey = KEY_LSHIFT;

	//vars that arent saved
	bool g_bWeaponConfigWindow = false;
	bool g_bDebugWindow = false;
	int g_iCurrentConfig = 0;
	

	bool g_bFrameStageNotify = true;
	bool g_bCreateMove = true;
	bool g_bOverrideView = true;
	bool g_bGetViewModelFOV = true;
	bool g_bPlaySound = true;
	bool g_bPaintTraverse = true;
	bool g_bDrawModelExecute = true;
	bool g_bEmitSound = true;
	bool g_bFireEventClientSide = true;
	bool g_bDoPostScreenSpaceEffects = true;
	bool g_bTraceRay = true;
	bool g_bSceneEnd = true;

	bool g_bFakelagPrediction = false;
	bool g_bAnimationFix = true;
	
	


	void LoadValues();
	void SaveValues() const;
	void Setup() const;
};

class AAVars : public singleton<AAVars>
{
public:
	bool g_bAAEnabled = false;
	bool g_bShowFakeAngle = false;
	bool g_bShowLBYAngle = false;
	ButtonCode_t g_kAAHotKey[4] = { KEY_UP, KEY_LEFT, KEY_DOWN, KEY_RIGHT };
	bool g_bAALinesEnabled = true;
	bool g_bEnemyAALinesEnabled = false;

	class AAVars_vals
	{
	public:
		bool g_bShouldFake = false; //todo: remove this value, if AA is enabled force fake
		int g_iAAPitch = 0;
		int g_iAAYaw = 0;
		int g_iAAFakeYaw = 0;
		int g_iMovingAA = 0;
		int g_iMovingAAOffset = 0;
		int g_iMovingJitterRange = 180;
		int g_iAARealYawOffset = 0;
		int g_iAAFakeYawOffset = 0;
		bool g_bAtTargets = true;
		bool g_bAntiLBY = true;
		float g_flAntiLBYOffset = 119.f;
		int g_iJitterRangeFirst = 180;
		int g_iJitterRangeSecond = 180;
		bool g_bFreeStanding = false;

		void SetupValues(int i) const;
		void LoadValues(int i);
		void SaveValues(int i) const;
	} aa[4];

	void LoadValues();
	void SaveValues() const;
	void Setup() const;
};

struct VisualVars : public singleton<VisualVars>
{
	struct ChamsVars
	{
		int g_bVisCheck = 0;

		float g_cVisibleEnemyColor[4] = { 0.f, 0.f, 1.f, 1.0f };
		float g_cHiddenEnemyColor[4] = { 0.2f, 1.f, 1.f, 1.0f };
		float g_cVisibleAllyColor[4] = { 0.2f, 0.f, 0.5f, 1.0f };
		float g_cHiddenAllyColor[4] = { 1.f, 0.f, 0.5f, 1.0f };

		bool g_bChamsShowTeammates = false;

		int g_iChamsArms = 0;	//0: off, 1: chams, 2: wireframe, 3: remove
		int g_iChamsWeapon = 0; //0: off, 1: chams, 2: wireframe, 3: remove
		int g_iChamsGloves = 0; //0: off, 1: chams, 2: wireframe, 3: remove
		bool g_bBacktrackChams = true;

		void LoadValues();
		void SaveValues();
		void Setup();
	} Chams;
	struct ESPVars
	{
		bool g_bFootstepESP = false;

		kBoxSettings g_iESPBox = BOX_OFF;
		bool g_bESPOverlay = false;

		bool g_bShowAmmo = false;
		bool g_bVisCheck = false;
		bool g_bESPEnabled = false;
		bool g_bShowSkeletons = false;
		bool g_bShowNames = false;
		kIconSettings g_bShowWeaponNames = ICON_OFF;
		bool g_bShowHealthbars = false;
		bool g_bShowArmorbars = false;
		bool g_bShowSnapLines = false;
		bool g_bShowGunBarrelLines = false;
		bool g_bShowTeammates = true;
		int g_iShowHitboxes = false;

		float g_cESPAllyColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
		float g_cESPEnemyColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };


		kIconSettings g_bShowGroundWeapons = ICON_OFF;
		kBoxSettings g_kGroundWeaponBoxMode = BOX_OFF;

		bool g_bShowGrenadePaths = false;
		kIconSettings g_bShowBomb = ICON_OFF;
		bool g_bShowPlantedBomb = false;
		bool g_bShowSpectators = false;

		bool g_bGrenadePrediction = false;

		bool g_bDormantESP = false;

		bool g_bBulletTracer = false;
		bool g_bDamage = false;

		void LoadValues();
		void SaveValues();
		void Setup();
	} ESP;
	struct GlowVars
	{
		bool g_bVisCheck = false;
		bool g_bGlowEnabled = false;
		bool g_bC4 = false;
		bool g_bWeapons = false;
		bool g_bEnemies = false;
		bool g_bTeammates = false;
		bool g_bProjectiles = false;
		bool g_bChicken = false;
		float g_flGlowScale = 0.75f;

		float g_cGlowColor[4] = { 255.f, 0.f, 0.f, 0.75f };
		float g_cGlowColorTeammates[4] = { 0.f, 0.f, 255.f, 0.75f };

		void LoadValues();
		void SaveValues();
		void Setup();
		//void SaveValues();
	} Glow;
	struct RemovalVars
	{
		bool g_bNoFlash = false;
		bool g_bNoScope = false;
		bool g_bNoSmoke = false;
		bool g_bNoTeammates = false;
		//bool g_bNoGroundWeapons = false;
		bool g_bNoRagdolls = false;

		void LoadValues();
		void SaveValues() const;
		void Setup() const;
	} Removals;

	int g_iFootstepESPLength = 50;
	bool g_bDrawSpread = false;
	bool g_bChams = false;
	bool g_bWallhack = true;
	int g_iCustomSky = 0;
	bool g_bGreyMode = false;
	bool g_bFullbright = false;
	bool g_bPsychedelic = false;
	bool g_bPixelated = false;
	bool g_bPostProcess = false;
	bool g_bRadar = false;

	bool g_bASUSWalls = false;
	float g_fASUSWallsScale = 0.75f;

	bool g_bWorldModulation = false;
	float g_cWorldModulationColor[4] = { 0.2549019607843137f, 0.2352941176470588f, 0.3137254901960784f, 1.f };

	void LoadValues();
	void SaveValues();
	void Setup();
};

struct Crosshair
{
	int g_iStyle = 0;
	bool g_bHitmarker = true;
	bool g_bHitmarkerSound = false;
	bool g_bDot = false;
	int g_iThickness = 3;
	int g_iLength = 12;
	int g_iGap = 4;
	bool g_bDynamic = false;
	float g_cColor[4] = { 1.f, 1.f, 1.f, 1.f };

	void LoadValues();
	void SaveValues();
	void Setup();
};

class MiscVars : public singleton<MiscVars>
{
public:
	Crosshair Crosshair;

	bool g_bShowRanks = false;
	bool g_bFOVChanger = false;
	int g_iFOVChangerValue = 90;
	bool g_bViewModelFOVChanger = false;
	int g_iViewModelFOVChangerValue = 68;
	bool g_bThirdPerson = false;
	int g_iThirdPersonFOV = 150;
	int g_iThirdPersonHeight = 0;
	bool g_bThirdPersonSpectator = false;

	bool g_bLocationSpammer = false;
	int g_iLocationSpammerMode = 0;

	bool g_bReportSpammer = false;
	bool g_bNameStealer = false;
	bool g_bChatSpammer = false;
	bool g_bAutoAccept = false;

	bool g_bClanTag = false;

	bool g_bEventLog = true;
	bool g_bShowFakeLagModel = false;

	bool g_bFakeLagOnKey = false;
	ButtonCode_t g_kLagKey = KEY_V;

	bool g_bBunnyHop = false;
	bool g_bAutoStrafe = false;
	bool g_bCircleStrafe = false;
	bool g_bRageStrafe = false;
	bool g_bAirCrouch = false;
	int g_iStrafeDirection = 0;

	bool g_bAutoKnife = false;
	bool g_bAutoWallTeamCheck = true;

	kFakelagSettings g_nFakeLag = FLAG_OFF;
	int g_iFakeLagAmount = 1;
	bool g_bFakelagOnLand = false;
	bool g_bFakelagOnlyOnMove = false;

	kBuyBotSecondarySettings g_nBuySecondary = BBS_NONE;
	kBuyBotPrimarySettings g_nBuyPrimary = BBP_NONE;

	void LoadValues();
	void SaveValues();
	void Setup();
};

struct WeaponConfigVars : public singleton<WeaponConfigVars>
{
	struct WeaponConfig_t
	{
		bool bEnabled = false;

		struct
		{
			ButtonCode_t g_kAimKey = MOUSE_LEFT;
			ButtonCode_t g_kTriggerKey = MOUSE_MIDDLE;
		} menu;

		struct
		{
			bool g_bIgnoreJumpingEnemy = true;
			bool g_bAimbotEnabled = false;
			bool g_bHitscanEnabled = true;
			int g_iHitbox = 0;
			int g_iFOV = 30;
			bool g_bAutoStop = false;
			bool g_bAutoDuck = false;
			bool g_bPSilent = false;
			bool g_bRCSEnabled = true;
			bool g_bSilentRCS = false;
			bool g_bStandaloneRCS = false;
			float g_flRCSPitchScale = 100.0f;
			float g_flRCSYawScale = 100.0f;
			float g_flSmoothX = .4f;
			float g_flSmoothY = .4f;

			//new values
			int g_iAimbotStyle = 0;
			int g_iSmoothType = 0;
			bool g_bSmokeCheck = true;
			bool g_bAutoAim = false;
		} legitbot;

		struct
		{
			bool g_bAutoFire = false;
			int g_iSilentAim = 0;
			float g_flPointScale = 0.65f;
			bool g_bAdaptiveFakeLag = false;
			bool g_bSmartHitscan = true;

			bool HitgroupSelections[8] =
			{
				true, //head
				true, //chest
				false, //upper arms
				false, //lower arms
				false, //upper legs
				false, //lower legs
				true, //hands
				true  //feet
			};

			bool g_bMultipoint = true;

			bool g_bHitscan = true;
			int g_iHitscanMode = 0;
			int g_iHitbox = 0;
			bool g_bAutoStop = false;
			bool g_bAutoStopOnKillShot = false;
			bool g_bAutoDuck = false;
			bool g_bAutoScope = true;
			bool g_bHitchance = false;
			float g_flHitchanceValue = 40.f;
			bool g_bAutoWall = false;
			int g_iMinimumDamage = 1;
			bool g_bAimStep = false;

		}ragebot;

		struct
		{
			bool g_bTriggerbotEnabled = false;
			bool g_bAutoshoot = false;
			bool g_bTriggerbotHead = true;
			bool g_bTriggerbotChest = false;
			bool g_bTriggerbotStomach = false;
			bool g_bTriggerbotArms = false;
			bool g_bTriggerbotLegs = false;
			bool g_bHitChance = false;
			float g_flHitchancePercentage = 90.f;
		} triggerbot;

		struct
		{
			bool g_bAutoPistol = false;
		} misc;

		struct
		{
			bool g_bBackTracking = false;
		} resolver;

		void LoadValues(std::string name);
		void SaveValues(std::string name);
		void Setup(std::string name);
	} WeaponConfig[65];

	void LoadValues();
	void SaveValues();
	void Setup();
};

struct ThemeVars : public singleton<ThemeVars>
{
	float col_Text[4] = { 0.784313725f, 0.784313725f, 0.784313725f, 1.f };
	float col_Primary[4] = { 0.3725490196f, 0.215686274f, 0.8431372549f, 1.f };
	float col_Secondary[4] = { 0.023529411f, 0.023529411f, 0.0235294f, 0.988235294f };
	float col_Tertiary[4] = { 0.0784313f, 0.078431372f, 0.078431372f, 1.f };

	void LoadValues();
	void SaveValues();
	void Setup();
};
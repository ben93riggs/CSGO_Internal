#pragma once
#include <Windows.h>

#define DAMAGE_NO		0
#define DAMAGE_EVENTS_ONLY	1	
#define DAMAGE_YES		2
#define DAMAGE_AIM		3

#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define	CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E' //< the egg sacks in the tunnels in ep2.
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
//#define CHAR_TEX_UNUSED		'J'
//#define CHAR_TEX_UNUSED		'K'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
//#define CHAR_TEX_UNUSED		'Q'
//#define CHAR_TEX_UNUSED		'R'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
//#define CHAR_TEX_UNUSED		'X'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z' //< wierd-looking jello effect for advisor shield.

// These are given to FindMaterial to reference the texture groups that show up on the
#define TEXTURE_GROUP_LIGHTMAP						"Lightmaps"
#define TEXTURE_GROUP_WORLD							"World textures"
#define TEXTURE_GROUP_MODEL							"Model textures"
#define TEXTURE_GROUP_VGUI							"VGUI textures"
#define TEXTURE_GROUP_PARTICLE						"Particle textures"
#define TEXTURE_GROUP_DECAL							"Decal textures"
#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
#define TEXTURE_GROUP_OTHER							"Other textures"
#define TEXTURE_GROUP_PRECACHED						"Precached"				// TODO: assign texture groups to the precached materials
#define TEXTURE_GROUP_CUBE_MAP						"CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					"RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					"Unaccounted textures"	// Textures that weren't assigned a texture group.
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		"Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			"Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			"Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			"Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					"DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					"ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					"Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				"Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			"RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					"Morph Targets"
#define TEXTURE_GROUP_STATIC_PROP					"StaticProp textures"

#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10			// alerts NPC, but doesn't do damage or bleed (1/100th damage)

#define MAXSTUDIOBONES				128		// total bones actually used
#define BONE_USED_BY_HITBOX			0x100	// bone (or child) is used by a hit box
#define BONE_USED_BY_ANYTHING		0x0007FF00

#define FL_ONGROUND		(1 << 0)	
#define FL_DUCKING		(1 << 1)
#define FL_WATERJUMP	(1 << 2)
#define	FL_ONTRAIN		(1 << 3)
#define	FL_INRAIN		(1 << 4)
#define	FL_FROZEN		(1 << 5)
#define	FL_ATCONTROLS	(1 << 6)
#define	FL_CLIENT		(1 << 7)
#define	FL_FAKECLIENT	(1 << 8)

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);

#define NETVAR(type, name, table, ...) type& name##() const { static DWORD _##name = GET_NETVAR(table, __VA_ARGS__); assert(_##name); return *(type*)((uintptr_t)this + _##name); }
#define PNETVAR(type, name, table, ...) type* name##() const { static DWORD _##name = GET_NETVAR(table, __VA_ARGS__); return (type*)((uintptr_t)this + _##name); }
#define NETVAR_OFFSET(type, name, offset, table, ...) type& name##() const { static DWORD _##name = GET_NETVAR(table, __VA_ARGS__) + offset; return *(type*)((uintptr_t)this + _##name); }
#define PNETVAR_OFFSET(type, name, offset, table, ...) type* name##() const { static DWORD _##name = GET_NETVAR(table, __VA_ARGS__) + offset; return (type*)((uintptr_t)this + _##name); }
#define NETVAR_STATIC(type, name, offset) type& name##() const { static DWORD _##name = offset; assert(_##name); return *(type*)((uintptr_t)this + _##name); }
#define PNETVAR_STATIC(type, name, offset) type* name##() const { static DWORD _##name = offset; return (type*)((uintptr_t)this + _##name); }

typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
typedef void* (*InstantiateInterfaceFn)();

typedef unsigned int VPANEL;
typedef void(*pfnDemoCustomDataCallback)(unsigned char *pData, size_t iSize);

template<typename FuncType>
FuncType CallVFunction(void* ppClass, DWORD index) {
	PDWORD pVTable = *(PDWORD*)ppClass;
	DWORD dwAddress = pVTable[index];
	return (FuncType)(dwAddress);
}

enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

struct CHudTexture
{
	char	szShortName[64];	//0x0000
	char	szTextureFile[64];	//0x0040
	bool	bRenderUsingFont;	//0x0080
	bool	bPrecached;			//0x0081
	char	cCharacterInFont;	//0x0082 
	BYTE	pad_0x0083;			//0x0083
	int		hFont;				//0x0084
	int		iTextureId;			//0x0088
	float	afTexCoords[4];		//0x008C
	int		iPosX[4];			//0x009C
};

enum class FontFeature 
{
	FONT_FEATURE_ANTIALIASED_FONTS = 1,
	FONT_FEATURE_DROPSHADOW_FONTS = 2,
	FONT_FEATURE_OUTLINE_FONTS = 6,
};

enum class FontDrawType 
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

enum class FontFlags : int 
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

enum ClientFrameStage_t 
{
	FRAME_UNDEFINED = -1,
	FRAME_START,								// (haven't run any frames yet)
	FRAME_NET_UPDATE_START,						// A network packet is being recieved
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,		// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,		// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_END,						// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_RENDER_START,							// We're about to start rendering the scene
	FRAME_RENDER_END							// We've finished rendering the scene.
};

enum class LifeState 
{
	LIFE_ALIVE = 0,		// alive
	LIFE_DYING = 1,		// playing death animation or still falling off of a ledge waiting to hit ground
	LIFE_DEAD = 2		// dead. lying still.
};

enum WeaponType
{
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SMG,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_HEAVY,
	WEAPONTYPE_SNIPER,
	WEAPONTYPE_OTHER,
	WEAPONTYPE_KNIFE,
	WEAPONTYPE_REVOLVER
};

enum EItemDefinitionIndex 
{
	weapon_none = 0,
	weapon_deagle = 1,
	weapon_elite = 2,
	weapon_fiveseven = 3,
	weapon_glock = 4,
	weapon_ak47 = 7,
	weapon_aug = 8,
	weapon_awp = 9,
	weapon_famas = 10,
	weapon_g3sg1 = 11,
	weapon_galilar = 13,
	weapon_m249 = 14,
	weapon_m4a4 = 16,
	weapon_mac10 = 17,
	weapon_p90 = 19,
	weapon_ump = 24,
	weapon_xm1014 = 25,
	weapon_bizon = 26,
	weapon_mag7 = 27,
	weapon_negev = 28,
	weapon_sawedoff = 29,
	weapon_tec9 = 30,
	weapon_taser = 31,
	weapon_hkp2000 = 32,
	weapon_mp7 = 33,
	weapon_mp9 = 34,
	weapon_nova = 35,
	weapon_p250 = 36,
	weapon_scar20 = 38,
	weapon_sg553 = 39,
	weapon_ssg08 = 40,
	weapon_knifegg = 41,
	weapon_knife = 42,
	weapon_flashbang = 43,
	weapon_hegrenade = 44,
	weapon_smokegrenade = 45,
	weapon_molotov = 46,
	weapon_decoy = 47,
	weapon_incgrenade = 48,
	weapon_c4 = 50,
	weapon_knife_t = 59,
	weapon_m4a1_silencer = 60,
	weapon_usp_silencer = 61,
	weapon_cz75a = 63,
	weapon_revolver = 64,
	weapon_knife_bayonet = 500,
	weapon_knife_flip = 505,
	weapon_knife_gut = 506,
	weapon_knife_karambit = 507,
	weapon_knife_m9_bayonet = 508,
	weapon_knife_tactical = 509,
	weapon_knife_falchion = 512,
	weapon_knife_survival_bowie = 514,
	weapon_knife_butterfly = 515,
	weapon_knife_push = 516
};

enum EClassIds
{
	CAK47 = 1,
	CBaseAnimating = 2,
	CBaseAnimatingOverlay = 3,
	C_BaseAttributableItem = 4,
	CBaseButton = 5,
	CBaseCombatCharacter = 6,
	C_BaseCombatWeapon = 7,
	CBaseCSGrenade = 8,
	CBaseCSGrenadeProjectile = 9,
	CBaseDoor = 10,
	CBaseEntity = 11,
	CBaseFlex = 12,
	CBaseGrenade = 13,
	CBaseParticleEntity = 14,
	CBasePlayer = 15,
	CBasePropDoor = 16,
	CBaseTeamObjectiveResource = 17,
	CBaseTempEntity = 18,
	CBaseToggle = 19,
	CBaseTrigger = 20,
	C_BaseViewModel = 21,
	CBaseVPhysicsTrigger = 22,
	CBaseWeaponWorldModel = 23,
	CBeam = 24,
	CBeamSpotlight = 25,
	CBoneFollower = 26,
	CBreakableProp = 27,
	CBreakableSurface = 28,
	CC4 = 29,
	CCascadeLight = 30,
	CChicken = 31,
	CColorCorrection = 32,
	CColorCorrectionVolume = 33,
	CCSGameRulesProxy = 34,
	CCSPlayer = 35,
	CCSPlayerResource = 36,
	CCSRagdoll = 37,
	CCSTeam = 38,
	CDEagle = 39,
	CDecoyGrenade = 40,
	CDecoyProjectile = 41,
	CDynamicLight = 42,
	CDynamicProp = 43,
	CEconEntity = 44,
	CEconWearable = 45,
	CEmbers = 46,
	CEntityDissolve = 47,
	CEntityFlame = 48,
	CEntityFreezing = 49,
	CEntityParticleTrail = 50,
	CEnvAmbientLight = 51,
	CEnvDetailController = 52,
	CEnvDOFController = 53,
	CEnvParticleScript = 54,
	CEnvProjectedTexture = 55,
	CEnvQuadraticBeam = 56,
	CEnvScreenEffect = 57,
	CEnvScreenOverlay = 58,
	CEnvTonemapController = 59,
	CEnvWind = 60,
	CFEPlayerDecal = 61,
	CFireCrackerBlast = 62,
	CFireSmoke = 63,
	CFireTrail = 64,
	CFish = 65,
	CFlashbang = 66,
	CFogController = 67,
	CFootstepControl = 68,
	CFunc_Dust = 69,
	CFunc_LOD = 70,
	CFuncAreaPortalWindow = 71,
	CFuncBrush = 72,
	CFuncConveyor = 73,
	CFuncLadder = 74,
	CFuncMonitor = 75,
	CFuncMoveLinear = 76,
	CFuncOccluder = 77,
	CFuncReflectiveGlass = 78,
	CFuncRotating = 79,
	CFuncSmokeVolume = 80,
	CFuncTrackTrain = 81,
	CGameRulesProxy = 82,
	CHandleTest = 83,
	CHEGrenade = 84,
	CHostage = 85,
	CHostageCarriableProp = 86,
	CIncendiaryGrenade = 87,
	CInferno = 88,
	CInfoLadderDismount = 89,
	CInfoOverlayAccessor = 90,
	CItem_Healthshot = 91,
	CItemDogtags = 92,
	CKnife = 93,
	CKnifeGG = 94,
	CLightGlow = 95,
	CMaterialModifyControl = 96,
	CMolotovGrenade = 97,
	CMolotovProjectile = 98,
	CMovieDisplay = 99,
	CParticleFire = 100,
	CParticlePerformanceMonitor = 101,
	CParticleSystem = 102,
	CPhysBox = 103,
	CPhysBoxMultiplayer = 104,
	CPhysicsProp = 105,
	CPhysicsPropMultiplayer = 106,
	CPhysMagnet = 107,
	CPlantedC4 = 108,
	CPlasma = 109,
	CPlayerResource = 110,
	CPointCamera = 111,
	CPointCommentaryNode = 112,
	CPointWorldText = 113,
	CPoseController = 114,
	CPostProcessController = 115,
	CPrecipitation = 116,
	CPrecipitationBlocker = 117,
	CPredictedViewModel = 118,
	CProp_Hallucination = 119,
	CPropDoorRotating = 120,
	CPropJeep = 121,
	CPropVehicleDriveable = 122,
	CRagdollManager = 123,
	CRagdollProp = 124,
	CRagdollPropAttached = 125,
	CRopeKeyframe = 126,
	CSCAR17 = 127,
	CSceneEntity = 128,
	CSensorGrenade = 129,
	CSensorGrenadeProjectile = 130,
	CShadowControl = 131,
	CSlideshowDisplay = 132,
	CSmokeGrenade = 133,
	CSmokeGrenadeProjectile = 134,
	CSmokeStack = 135,
	CSpatialEntity = 136,
	CSpotlightEnd = 137,
	CSprite = 138,
	CSpriteOriented = 139,
	CSpriteTrail = 140,
	CStatueProp = 141,
	CSteamJet = 142,
	CSun = 143,
	CSunlightShadowControl = 144,
	CTeam = 145,
	CTeamplayRoundBasedRulesProxy = 146,
	CTEArmorRicochet = 147,
	CTEBaseBeam = 148,
	CTEBeamEntPoint = 149,
	CTEBeamEnts = 150,
	CTEBeamFollow = 151,
	CTEBeamLaser = 152,
	CTEBeamPoints = 153,
	CTEBeamRing = 154,
	CTEBeamRingPoint = 155,
	CTEBeamSpline = 156,
	CTEBloodSprite = 157,
	CTEBloodStream = 158,
	CTEBreakModel = 159,
	CTEBSPDecal = 160,
	CTEBubbles = 161,
	CTEBubbleTrail = 162,
	CTEClientProjectile = 163,
	CTEDecal = 164,
	CTEDust = 165,
	CTEDynamicLight = 166,
	CTEEffectDispatch = 167,
	CTEEnergySplash = 168,
	CTEExplosion = 169,
	CTEFireBullets = 170,
	CTEFizz = 171,
	CTEFootprintDecal = 172,
	CTEFoundryHelpers = 173,
	CTEGaussExplosion = 174,
	CTEGlowSprite = 175,
	CTEImpact = 176,
	CTEKillPlayerAttachments = 177,
	CTELargeFunnel = 178,
	CTEMetalSparks = 179,
	CTEMuzzleFlash = 180,
	CTEParticleSystem = 181,
	CTEPhysicsProp = 182,
	CTEPlantBomb = 183,
	CTEPlayerAnimEvent = 184,
	CTEPlayerDecal = 185,
	CTEProjectedDecal = 186,
	CTERadioIcon = 187,
	CTEShatterSurface = 188,
	CTEShowLine = 189,
	CTesla = 190,
	CTESmoke = 191,
	CTESparks = 192,
	CTESprite = 193,
	CTESpriteSpray = 194,
	CTest_ProxyToggle_Networkable = 194,
	CTestTraceline = 196,
	CTEWorldDecal = 197,
	CTriggerPlayerMovement = 198,
	CTriggerSoundOperator = 199,
	CVGuiScreen = 200,
	CVoteController = 201,
	CWaterBullet = 202,
	CWaterLODControl = 203,
	CWeaponAug = 204,
	CWeaponAWP = 205,
	CWeaponBaseItem = 206,
	CWeaponBizon = 207,
	CWeaponCSBase = 208,
	CWeaponCSBaseGun = 209,
	CWeaponCycler = 210,
	CWeaponElite = 211,
	CWeaponFamas = 212,
	CWeaponFiveSeven = 213,
	CWeaponG3SG1 = 214,
	CWeaponGalil = 215,
	CWeaponGalilAR = 216,
	CWeaponGlock = 217,
	CWeaponHKP2000 = 218,
	CWeaponM249 = 219,
	CWeaponM3 = 220,
	CWeaponM4A1 = 221,
	CWeaponMAC10 = 222,
	CWeaponMag7 = 223,
	CWeaponMP5Navy = 224,
	CWeaponMP7 = 225,
	CWeaponMP9 = 226,
	CWeaponNegev = 227,
	CWeaponNOVA = 228,
	CWeaponP228 = 229,
	CWeaponP250 = 230,
	CWeaponP90 = 231,
	CWeaponSawedoff = 232,
	CWeaponSCAR20 = 233,
	CWeaponScout = 234,
	CWeaponSG550 = 235,
	CWeaponSG552 = 236,
	CWeaponSG556 = 237,
	CWeaponSSG08 = 238,
	CWeaponTaser = 239,
	CWeaponTec9 = 240,
	CWeaponTMP = 241,
	CWeaponUMP45 = 242,
	CWeaponUSP = 243,
	CWeaponXM1014 = 244,
	CWorld = 245,
	DustTrail = 246,
	MovieExplosion = 247,
	ParticleSmokeGrenade = 248,
	RocketTrail = 249,
	SmokeTrail = 250,
	SporeExplosion = 251,
	SporeTrail = 252
};

enum class Bone : int
{
	INVALID = -1,
	BONE_PELVIS = 0,
	LEAN_ROOT,
	CAM_DRIVER,
	BONE_HIP,
	BONE_LOWER_SPINAL_COLUMN,
	BONE_MIDDLE_SPINAL_COLUMN,
	BONE_UPPER_SPINAL_COLUMN,
	BONE_NECK,
	BONE_HEAD,
};

enum HitscanHitgroup
{
	GROUP_HEAD,
	GROUP_CHEST,
	GROUP_UPPER_ARMS,
	GROUP_LOWER_ARMS,
	GROUP_UPPER_LEGS,
	GROUP_LOWER_LEGS,
	GROUP_HANDS,
	GROUP_FEET
};

enum CSGOHitboxID
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_PELVIS,
	HITBOX_BELLY,
	HITBOX_THORAX,
	HITBOX_LOWER_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};

enum ECSPlayerBones 
{
	INVALID = -1,
	PELVIS = 0U,
	LEAN_ROOT = 1U,
	CAM_DRIVER = 2U,
	SPINE_0 = 3U,
	SPINE_1 = 4U,
	SPINE_2 = 5U,
	SPINE_3 = 6U,
	NECK_0 = 7U,
	HEAD_0 = 8U,
	CLAVICLE_L = 9U,
	ARM_UPPER_L = 10U,
	ARM_LOWER_L = 11U,
	HAND_L = 12U,
	FINGER_MIDDLE_META_L = 13U,
	FINGER_MIDDLE_0_L = 14U,
	FINGER_MIDDLE_1_L = 15U,
	FINGER_MIDDLE_2_L = 16U,
	FINGER_PINKY_META_L = 17U,
	FINGER_PINKY_0_L = 18U,
	FINGER_PINKY_1_L = 19U,
	FINGER_PINKY_2_L = 20U,
	FINGER_INDEX_META_L = 21U,
	FINGER_INDEX_0_L = 22U,
	FINGER_INDEX_1_L = 23U,
	FINGER_INDEX_2_L = 24U,
	FINGER_THUMB_0_L = 25U,
	FINGER_THUMB_1_L = 26U,
	FINGER_THUMB_2_L = 27U,
	FINGER_RING_META_L = 28U,
	FINGER_RING_0_L = 29U,
	FINGER_RING_1_L = 30U,
	FINGER_RING_2_L = 31U,
	WEAPON_HAND_L = 32U,
	ARM_LOWER_L_TWIST = 33U,
	ARM_LOWER_L_TWIST1 = 34U,
	ARM_UPPER_L_TWIST = 35U,
	ARM_UPPER_L_TWIST1 = 36U,
	CLAVICLE_R = 37U,
	ARM_UPPER_R = 38U,
	ARM_LOWER_R = 39U,
	HAND_R = 40U,
	FINGER_MIDDLE_META_R = 41U,
	FINGER_MIDDLE_0_R = 42U,
	FINGER_MIDDLE_1_R = 43U,
	FINGER_MIDDLE_2_R = 44U,
	FINGER_PINKY_META_R = 45U,
	FINGER_PINKY_0_R = 46U,
	FINGER_PINKY_1_R = 47U,
	FINGER_PINKY_2_R = 48U,
	FINGER_INDEX_META_R = 49U,
	FINGER_INDEX_0_R = 50U,
	FINGER_INDEX_1_R = 51U,
	FINGER_INDEX_2_R = 52U,
	FINGER_THUMB_0_R = 53U,
	FINGER_THUMB_1_R = 54U,
	FINGER_THUMB_2_R = 55U,
	FINGER_RING_META_R = 56U,
	FINGER_RING_0_R = 57U,
	FINGER_RING_1_R = 58U,
	FINGER_RING_2_R = 59U,
	WEAPON_HAND_R = 60U,
	ARM_LOWER_R_TWIST = 61U,
	ARM_LOWER_R_TWIST1 = 62U,
	ARM_UPPER_R_TWIST = 63U,
	ARM_UPPER_R_TWIST1 = 64U,
	LEG_UPPER_L = 65U,
	LEG_LOWER_L = 66U,
	ANKLE_L = 67U,
	BALL_L = 68U,
	LFOOT_LOCK = 69U,
	LEG_UPPER_L_TWIST = 70U,
	LEG_UPPER_L_TWIST1 = 71U,
	LEG_UPPER_R = 72U,
	LEG_LOWER_R = 73U,
	ANKLE_R = 74U,
	BALL_R = 75U,
	RFOOT_LOCK = 76U,
	LEG_UPPER_R_TWIST = 77U,
	LEG_UPPER_R_TWIST1 = 78U,
	FINGER_PINKY_L_END = 79U,
	FINGER_PINKY_R_END = 80U,
	VALVEBIPED_WEAPON_BONE = 81U,
	LH_IK_DRIVER = 82U,
	PRIMARY_JIGGLE_JNT = 83U,
};
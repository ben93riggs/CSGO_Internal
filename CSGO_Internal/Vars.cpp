#include "Vars.h"
#include "Globals.h"

void MenuVars::LoadValues()
{
	g_bHideTitle = CConfig::get().GetValuePtr(XorStr("Menu"), "Hide Title")->m_bValue;
	g_iCurrentAimbotMode = CConfig::get().GetValuePtr(XorStr("Menu"), "Aimbot Mode")->m_iValue;

	g_kCircleStrafeKey = (ButtonCode_t)CConfig::get().GetValuePtr(XorStr("Menu"), XorStr("Circle Strafe Key"))->m_iValue;
	g_kThirdPersonKey = (ButtonCode_t)CConfig::get().GetValuePtr(XorStr("Menu"), XorStr("ThirdPerson Key"))->m_iValue;
	g_bResolveYaw = CConfig::get().GetValuePtr(XorStr("Resolver"), XorStr("Resolve Yaw"))->m_bValue;
	g_kOverrideKey = (ButtonCode_t)CConfig::get().GetValuePtr(XorStr("Resolver"), XorStr("Override Key"))->m_iValue;
	g_kFakeWalkKey = (ButtonCode_t)CConfig::get().GetValuePtr(XorStr("AntiAim"), XorStr("Fakewalk Key"))->m_iValue;
	g_bFakewalk = CConfig::get().GetValuePtr(XorStr("AntiAim"), XorStr("Fakewalk"))->m_bValue;
	g_bBackTrackExploit = CConfig::get().GetValuePtr(XorStr("Resolver"), XorStr("Backtrack Exploit"))->m_bValue;
}

void MenuVars::SaveValues() const
{
	CConfig::get().GetValuePtr(XorStr("Menu"), XorStr("Hide Title"))->m_bValue = g_bHideTitle;
	CConfig::get().GetValuePtr(XorStr("Menu"), XorStr("Aimbot Mode"))->m_iValue = g_iCurrentAimbotMode;
	CConfig::get().GetValuePtr(XorStr("Menu"), XorStr("Circle Strafe Key"))->m_iValue = g_kCircleStrafeKey;
	CConfig::get().GetValuePtr(XorStr("Menu"), XorStr("ThirdPerson Key"))->m_iValue = g_kThirdPersonKey;
	CConfig::get().GetValuePtr(XorStr("Resolver"), XorStr("Override Key"))->m_iValue = g_kOverrideKey;
	CConfig::get().GetValuePtr(XorStr("Resolver"), XorStr("Resolve Yaw"))->m_bValue = g_bResolveYaw;
	CConfig::get().GetValuePtr(XorStr("Resolver"), XorStr("Backtrack Exploit"))->m_bValue = g_bBackTrackExploit;

	CConfig::get().GetValuePtr(XorStr("AntiAim"), XorStr("Fakewalk Key"))->m_iValue = g_kFakeWalkKey;
	CConfig::get().GetValuePtr(XorStr("AntiAim"), XorStr("Fakewalk"))->m_bValue = g_bFakewalk;
}

void MenuVars::Setup() const
{
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Hide Title"), ConfigValue(g_bHideTitle, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Aimbot Mode"), ConfigValue(g_iCurrentAimbotMode, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Circle Strafe Key"), ConfigValue(g_kCircleStrafeKey, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Menu"), XorStr("ThirdPerson Key"), ConfigValue(g_kThirdPersonKey, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Resolver"), XorStr("Override Key"), ConfigValue(g_kOverrideKey, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Resolver"), XorStr("Resolve Yaw"), ConfigValue(g_bResolveYaw, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Resolver"), XorStr("Backtrack Exploit"), ConfigValue(g_bBackTrackExploit, CONFIG_BOOL));

	CConfig::get().SetValue(XorStr("AntiAim"), XorStr("Fakewalk Key"), ConfigValue(g_kFakeWalkKey, CONFIG_INT));
	CConfig::get().SetValue(XorStr("AntiAim"), XorStr("Fakewalk"), ConfigValue(g_bFakewalk, CONFIG_BOOL));
}

void VisualVars::ChamsVars::LoadValues()
{
	CConfig::LoadColorFromConfig(g_cVisibleAllyColor, CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Visible Ally Color"))->m_colorValue);
	CConfig::LoadColorFromConfig(g_cHiddenAllyColor, CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Hidden Ally Color"))->m_colorValue);
	CConfig::LoadColorFromConfig(g_cVisibleEnemyColor, CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Visible Enemy Color"))->m_colorValue);
	CConfig::LoadColorFromConfig(g_cHiddenEnemyColor, CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Hidden Enemy Color"))->m_colorValue);

	g_bBacktrackChams = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Backtrack Chams"))->m_bValue;

	g_iChamsArms = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chams Arms"))->m_iValue;
	g_iChamsWeapon = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chams Weapon"))->m_iValue;
	g_iChamsGloves = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chams Gloves"))->m_iValue;
	g_bChamsShowTeammates = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chams Team Only"))->m_bValue;
}

void VisualVars::ChamsVars::SaveValues()
{
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Visible Ally Color"), ConfigValue(g_cVisibleAllyColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Hidden Ally Color"), ConfigValue(g_cHiddenAllyColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Visible Enemy Color"), ConfigValue(g_cVisibleEnemyColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Hidden Enemy Color"), ConfigValue(g_cHiddenEnemyColor, CONFIG_COLOR));
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chams Arms"))->m_iValue = g_iChamsArms;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chams Weapon"))->m_iValue = g_iChamsWeapon;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chams Gloves"))->m_iValue = g_iChamsGloves;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chams Team Only"))->m_bValue = g_bChamsShowTeammates;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Backtrack Chams"))->m_bValue = g_bBacktrackChams;
}

void VisualVars::ChamsVars::Setup()
{
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Visible Ally Color"), ConfigValue(g_cVisibleAllyColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Hidden Ally Color"), ConfigValue(g_cHiddenAllyColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Visible Enemy Color"), ConfigValue(g_cVisibleEnemyColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Hidden Enemy Color"), ConfigValue(g_cHiddenEnemyColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Chams Arms"), ConfigValue(g_iChamsArms, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Chams Weapon"), ConfigValue(g_iChamsWeapon, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Chams Gloves"), ConfigValue(g_iChamsGloves, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Chams Team Only"), ConfigValue(g_bChamsShowTeammates, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Backtrack Chams"), ConfigValue(g_bBacktrackChams, CONFIG_BOOL));
}

void VisualVars::ESPVars::LoadValues()
{
	g_bShowAmmo = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Ammo"))->m_bValue;
	g_bFootstepESP = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ESP Footstep ESP"))->m_bValue;
	g_bESPEnabled = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ESP"))->m_bValue;
	g_bESPOverlay = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ESP Overlay"))->m_bValue;
	g_iESPBox = (kBoxSettings)CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Boxes"))->m_iValue;
	g_iShowHitboxes = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Hitboxes"))->m_iValue;
	g_bShowSkeletons = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Skeletons"))->m_bValue;
	g_bShowNames = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Names"))->m_bValue;
	g_bShowWeaponNames = (kIconSettings)CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Weapon Name"))->m_iValue;
	g_bShowHealthbars = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Healthbars"))->m_bValue;
	g_bShowArmorbars = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Armorbars"))->m_bValue;
	g_bShowSnapLines = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Snap Lines"))->m_bValue;
	g_bShowGunBarrelLines = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Gun Barrel Lines"))->m_bValue;
	g_bShowTeammates = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Teammates"))->m_bValue;
	g_bDormantESP = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Dormant Players"))->m_bValue;

	CConfig::LoadColorFromConfig(g_cESPAllyColor, CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ESP Ally Color"))->m_colorValue);
	CConfig::LoadColorFromConfig(g_cESPEnemyColor, CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ESP Enemy Color"))->m_colorValue);

	g_bShowGroundWeapons = (kIconSettings)CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Dropped Weapons"))->m_iValue;
	g_bShowGrenadePaths = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Grenade Paths"))->m_bValue;
	g_bGrenadePrediction = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Grenade Predicted Path"))->m_bValue;
	g_bShowBomb = (kIconSettings)CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Bomb"))->m_iValue;
	g_bShowPlantedBomb = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Planted Bomb + Damage"))->m_bValue;
	g_bShowSpectators = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Spectators"))->m_bValue;

	g_bBulletTracer = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Bullet Tracers"))->m_bValue;

	g_bDamage = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Damage Indicators"))->m_bValue;
}

void VisualVars::ESPVars::SaveValues()
{
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("ESP Ally Color"), ConfigValue(g_cESPAllyColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("ESP Enemy Color"), ConfigValue(g_cESPEnemyColor, CONFIG_COLOR));

	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ESP"))->m_bValue = g_bESPEnabled;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ESP Overlay"))->m_bValue = g_bESPOverlay;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Hitboxes"))->m_iValue = g_iShowHitboxes;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Boxes"))->m_iValue = g_iESPBox;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Skeletons"))->m_bValue = g_bShowSkeletons;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Names"))->m_bValue = g_bShowNames;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Weapon Name"))->m_bValue = g_bShowWeaponNames;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Healthbars"))->m_bValue = g_bShowHealthbars;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Armorbars"))->m_bValue = g_bShowArmorbars;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Ammo"))->m_bValue = g_bShowAmmo;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Snap Lines"))->m_bValue = g_bShowSnapLines;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Gun Barrel Lines"))->m_bValue = g_bShowGunBarrelLines;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Teammates"))->m_bValue = g_bShowTeammates;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ESP Footstep ESP"))->m_bValue = g_bFootstepESP;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Dropped Weapons"))->m_bValue = g_bShowGroundWeapons;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Grenade Paths"))->m_bValue = g_bShowGrenadePaths;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Grenade Predicted Path"))->m_bValue = g_bGrenadePrediction;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Bomb"))->m_bValue = g_bShowBomb;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Planted Bomb + Damage"))->m_bValue = g_bShowPlantedBomb;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Dormant Players"))->m_bValue = g_bDormantESP;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Damage Indicators"))->m_bValue = g_bDamage;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Spectators"))->m_bValue = g_bShowSpectators;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Show Bullet Tracers"))->m_bValue = g_bBulletTracer;
}

void VisualVars::ESPVars::Setup()
{
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("ESP Ally Color"), ConfigValue(g_cESPAllyColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("ESP Enemy Color"), ConfigValue(g_cESPEnemyColor, CONFIG_COLOR));

	CConfig::get().SetValue(XorStr("Visuals"), XorStr("ESP"), ConfigValue(g_bESPEnabled, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("ESP Overlay"), ConfigValue(g_bESPOverlay, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Hitboxes"), ConfigValue(g_iShowHitboxes, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Boxes"), ConfigValue(g_iESPBox, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Skeletons"), ConfigValue(g_bShowSkeletons, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Names"), ConfigValue(g_bShowNames, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Weapon Name"), ConfigValue(g_bShowWeaponNames, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Healthbars"), ConfigValue(g_bShowHealthbars, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Armorbars"), ConfigValue(g_bShowArmorbars, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Ammo"), ConfigValue(g_bShowAmmo, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Snap Lines"), ConfigValue(g_bShowSnapLines, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Gun Barrel Lines"), ConfigValue(g_bShowGunBarrelLines, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Teammates"), ConfigValue(g_bShowTeammates, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("ESP Footstep ESP"), ConfigValue(g_bFootstepESP, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Dropped Weapons"), ConfigValue(g_bShowGroundWeapons, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Grenade Paths"), ConfigValue(g_bShowGrenadePaths, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Grenade Predicted Path"), ConfigValue(g_bGrenadePrediction, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Bomb"), ConfigValue(g_bShowBomb, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Planted Bomb + Damage"), ConfigValue(g_bShowPlantedBomb, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Dormant Players"), ConfigValue(g_bDormantESP, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Damage Indicators"), ConfigValue(g_bDamage, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Spectators"), ConfigValue(g_bShowSpectators, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Show Bullet Tracers"), ConfigValue(g_bBulletTracer, CONFIG_BOOL));
}

void VisualVars::GlowVars::LoadValues()
{
	g_bGlowEnabled = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Glow Enabled"))->m_bValue;
	g_bC4 = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("C4 Glow"))->m_bValue;
	g_bWeapons = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Weapon Glow"))->m_bValue;
	g_bEnemies = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Enemy Glow"))->m_bValue;
	g_bTeammates = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Teammate Glow"))->m_bValue;
	g_bProjectiles = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Grenade Glow"))->m_bValue;
	g_bChicken = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chicken Glow"))->m_bValue;
	//g_flGlowScale = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Glow Scale"))->m_flValue;

	CConfig::LoadColorFromConfig(g_cGlowColor, CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Glow Color Enemies"))->m_colorValue);
	CConfig::LoadColorFromConfig(g_cGlowColorTeammates, CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Glow Color Teammates"))->m_colorValue);

}

void VisualVars::GlowVars::SaveValues()
{
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Glow Color Enemies"), ConfigValue(g_cGlowColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Glow Color Teammates"), ConfigValue(g_cGlowColorTeammates, CONFIG_COLOR));

	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Glow Enabled"))->m_bValue = g_bGlowEnabled;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("C4 Glow"))->m_bValue = g_bC4;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Weapon Glow"))->m_bValue = g_bWeapons;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Enemy Glow"))->m_bValue = g_bEnemies;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Teammate Glow"))->m_bValue = g_bTeammates;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Grenade Glow"))->m_bValue = g_bProjectiles;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chicken Glow"))->m_bValue = g_bChicken;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Glow Scale"))->m_flValue = g_flGlowScale;
}

void VisualVars::GlowVars::Setup()
{
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Glow Color Enemies"), ConfigValue(g_cGlowColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Glow Color Teammates"), ConfigValue(g_cGlowColorTeammates, CONFIG_COLOR));

	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Glow Enabled"), ConfigValue(g_bGlowEnabled, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("C4 Glow"), ConfigValue(g_bC4, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Weapon Glow"), ConfigValue(g_bWeapons, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Enemy Glow"), ConfigValue(g_bEnemies, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Teammate Glow"), ConfigValue(g_bTeammates, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Grenade Glow"), ConfigValue(g_bProjectiles, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Chicken Glow"), ConfigValue(g_bChicken, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Glow Scale"), ConfigValue(g_flGlowScale, CONFIG_FLOAT));
}

void VisualVars::RemovalVars::LoadValues()
{
	g_bNoFlash = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Flash"))->m_bValue;
	g_bNoSmoke = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Smoke"))->m_bValue;

	g_bNoScope = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Scope Overlay"))->m_bValue;

	g_bNoTeammates = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Teammates"))->m_bValue;

	//g_bNoGroundWeapons = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Ground Weapons"))->m_bValue;

	g_bNoRagdolls = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Ragdolls"))->m_bValue;
}

void VisualVars::RemovalVars::SaveValues() const
{
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Scope Overlay"))->m_bValue = g_bNoScope;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Smoke"))->m_bValue = g_bNoSmoke;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Flash"))->m_bValue = g_bNoFlash;

	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Teammates"))->m_bValue = g_bNoTeammates;

	//CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Ground Weapons"))->m_bValue = g_bNoGroundWeapons;

	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Ragdolls"))->m_bValue = g_bNoRagdolls;
}

void VisualVars::RemovalVars::Setup() const
{
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("No Scope Overlay"), ConfigValue(g_bNoScope, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("No Smoke"), ConfigValue(g_bNoSmoke, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("No Flash"), ConfigValue(g_bNoFlash, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("No Teammates"), ConfigValue(g_bNoTeammates, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("No Ragdolls"), ConfigValue(g_bNoRagdolls, CONFIG_BOOL));
}

void VisualVars::LoadValues()
{
	Chams.LoadValues();
	ESP.LoadValues();
	Glow.LoadValues();
	Removals.LoadValues();

	g_bDrawSpread = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Draw Spread Cone"))->m_bValue;
	g_bChams = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chams"))->m_bValue;
	g_bWallhack = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Wallhack"))->m_bValue;
	//g_bWireframe = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("WireFrame"))->m_bValue;
	g_iFootstepESPLength = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Footstep ESP Length"))->m_iValue;

	g_bRadar = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Radar"))->m_bValue;

	g_bGreyMode = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Grey Mode"))->m_bValue;
	g_bFullbright = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Fullbright"))->m_bValue;
	g_bPsychedelic = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Psychedelic"))->m_bValue;
	g_bPixelated = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Pixelated"))->m_bValue;
	g_bPostProcess = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Post-Processing"))->m_bValue;

	g_bWorldModulation = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("World Modulation"))->m_bValue;
	CConfig::LoadColorFromConfig(g_cWorldModulationColor, CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("World Modulation Color"))->m_colorValue);
	g_iCustomSky = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Custom Sky"))->m_iValue;

	g_bASUSWalls = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ASUS Props"))->m_bValue;
	g_fASUSWallsScale = CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ASUS Props Scale"))->m_flValue;
}

void VisualVars::SaveValues()
{
	Chams.SaveValues();
	ESP.SaveValues();
	Glow.SaveValues();
	Removals.SaveValues();

	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Draw Spread Cone"))->m_bValue = g_bDrawSpread;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Chams"))->m_bValue = g_bChams;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Wallhack"))->m_bValue = g_bWallhack;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Footstep ESP Length"))->m_iValue = g_iFootstepESPLength;
	
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Grey Mode"))->m_bValue = g_bGreyMode;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Fullbright"))->m_bValue = g_bFullbright;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Psychedelic"))->m_bValue = g_bPsychedelic;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Pixelated"))->m_bValue = g_bPixelated;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("No Post-Processing"))->m_bValue = g_bPostProcess;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("World Modulation"))->m_bValue = g_bWorldModulation;
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("World Modulation Color"), ConfigValue(g_cWorldModulationColor, CONFIG_COLOR));
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Custom Sky"))->m_iValue = g_iCustomSky;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ASUS Props"))->m_bValue = g_bASUSWalls;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("ASUS Props Scale"))->m_flValue = g_fASUSWallsScale;
	CConfig::get().GetValuePtr(XorStr("Visuals"), XorStr("Radar"))->m_bValue = g_bRadar;
}

void VisualVars::Setup()
{
	Chams.Setup();
	ESP.Setup();
	Glow.Setup();
	Removals.Setup();

	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Draw Spread Cone"), ConfigValue(g_bDrawSpread, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Chams"), ConfigValue(g_bChams, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Wallhack"), ConfigValue(g_bWallhack, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Footstep ESP Length"), ConfigValue(g_iFootstepESPLength, CONFIG_INT));

	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Grey Mode"), ConfigValue(g_bGreyMode, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Fullbright"), ConfigValue(g_bFullbright, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Psychedelic"), ConfigValue(g_bPsychedelic, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Pixelated"), ConfigValue(g_bPixelated, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("No Post-Processing"), ConfigValue(g_bPostProcess, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("World Modulation"), ConfigValue(g_bWorldModulation, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("World Modulation Color"), ConfigValue(g_cWorldModulationColor, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Custom Sky"), ConfigValue(g_iCustomSky, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("ASUS Props"), ConfigValue(g_bASUSWalls, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("ASUS Props Scale"), ConfigValue(g_fASUSWallsScale, CONFIG_FLOAT));
	CConfig::get().SetValue(XorStr("Visuals"), XorStr("Radar"), ConfigValue(g_bRadar, CONFIG_BOOL));
}

void Crosshair::LoadValues()
{
	g_bHitmarker = CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Hitmarkers"))->m_bValue;
	g_bHitmarkerSound = CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Hitmarker Sound"))->m_bValue;
	g_bDot = CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Dot"))->m_bValue;
	g_iStyle = CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Style"))->m_iValue;
	g_iThickness = CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Thickness"))->m_iValue;
	g_iLength = CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Length"))->m_iValue;
	g_iGap = CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Gap"))->m_iValue;
	g_bDynamic = CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Dynamic"))->m_bValue;
	CConfig::LoadColorFromConfig(g_cColor, CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Color"))->m_colorValue);
}

void Crosshair::SaveValues()
{
	CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Hitmarkers"))->m_bValue = g_bHitmarker;
	CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Hitmarker Sound"))->m_bValue = g_bHitmarkerSound;
	CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Dot"))->m_bValue = g_bDot;
	CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Style"))->m_iValue = g_iStyle;
	CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Thickness"))->m_iValue = g_iThickness;
	CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Length"))->m_iValue = g_iLength;
	CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Gap"))->m_iValue = g_iGap;
	CConfig::get().GetValuePtr(XorStr("Crosshair"), XorStr("Dynamic"))->m_bValue = g_bDynamic;
	CConfig::get().SetValue(XorStr("Crosshair"), XorStr("Color"), ConfigValue(g_cColor, CONFIG_COLOR));
}

void Crosshair::Setup()
{
	CConfig::get().SetValue(XorStr("Crosshair"), XorStr("Hitmarkers"), ConfigValue(g_bHitmarker, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Crosshair"), XorStr("Hitmarker Sound"), ConfigValue(g_bHitmarkerSound, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Crosshair"), XorStr("Dot"), ConfigValue(g_bDot, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Crosshair"), XorStr("Style"), ConfigValue(g_iStyle, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Crosshair"), XorStr("Thickness"), ConfigValue(g_iThickness, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Crosshair"), XorStr("Length"), ConfigValue(g_iLength, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Crosshair"), XorStr("Gap"), ConfigValue(g_iGap, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Crosshair"), XorStr("Dynamic"), ConfigValue(g_bDynamic, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Crosshair"), XorStr("Color"), ConfigValue(g_cColor, CONFIG_COLOR));
}

void MiscVars::LoadValues()
{
	Crosshair.LoadValues();

	g_bShowRanks = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Show Ranks"))->m_bValue;
	g_bBunnyHop = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Bunny Hop"))->m_bValue;
	g_bAutoStrafe = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Auto Strafe"))->m_bValue;
	g_bCircleStrafe = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Circle Strafe"))->m_bValue;
	g_bAirCrouch = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Air Crouch"))->m_bValue;

	g_bRageStrafe = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Rage Strafe"))->m_bValue;
	g_iStrafeDirection = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Strafe Direction"))->m_iValue;

	g_bFOVChanger = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("FOV Changer"))->m_bValue;
	g_iFOVChangerValue = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("FOV"))->m_iValue;
	g_bThirdPerson = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Thirdperson"))->m_bValue;
	g_bThirdPersonSpectator = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Thirdperson Spectate"))->m_bValue;
	g_bLocationSpammer = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Location Spammer"))->m_bValue;
	g_iLocationSpammerMode = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Location Spammer Mode"))->m_iValue;
	g_bReportSpammer = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Report Spammer"))->m_bValue;
	g_bNameStealer = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Name Stealer"))->m_bValue;
	g_bChatSpammer = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Chat Spam"))->m_bValue;
	g_bAutoAccept = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Auto Accept"))->m_bValue;
	g_bClanTag = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Clan Tag"))->m_bValue;
	//g_bAirStuck = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("AirStuck"))->m_bValue;
	g_bEventLog = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Event Logger"))->m_bValue;

	g_bAutoKnife = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Auto Knife"))->m_bValue;

	g_bShowFakeLagModel = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Show Fake Lag Model"))->m_bValue;

	g_bFakeLagOnKey = (ButtonCode_t)CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake Lag On Key"))->m_bValue;
	g_kLagKey = (ButtonCode_t)CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake Lag Hotkey"))->m_iValue;
	g_nFakeLag = (kFakelagSettings)CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake Lag"))->m_iValue;
	g_iFakeLagAmount = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake Lag Amount"))->m_iValue;
	g_bFakelagOnLand = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake Lag On Land"))->m_bValue;
	g_bFakelagOnlyOnMove = CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake lag only when moving"))->m_bValue;
}

void MiscVars::SaveValues()
{
	Crosshair.SaveValues();

	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Show Ranks"))->m_bValue = g_bShowRanks;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Bunny Hop"))->m_bValue = g_bBunnyHop;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Auto Strafe"))->m_bValue = g_bAutoStrafe;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Circle Strafe"))->m_bValue = g_bCircleStrafe;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Rage Strafe"))->m_bValue = g_bRageStrafe;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Strafe Direction"))->m_iValue = g_iStrafeDirection;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Air Crouch"))->m_bValue = g_bAirCrouch;

	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("FOV Changer"))->m_bValue = g_bFOVChanger;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("FOV"))->m_iValue = g_iFOVChangerValue;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Thirdperson"))->m_bValue = g_bThirdPerson;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Thirdperson Spectate"))->m_bValue = g_bThirdPersonSpectator;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Location Spammer"))->m_bValue = g_bLocationSpammer;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Location Spammer Mode"))->m_iValue = g_iLocationSpammerMode;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Report Spammer"))->m_bValue = g_bReportSpammer;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Name Stealer"))->m_bValue = g_bNameStealer;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Chat Spam"))->m_bValue = g_bChatSpammer;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Auto Accept"))->m_bValue = g_bAutoAccept;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Clan Tag"))->m_bValue = g_bClanTag;
	//CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("AirStuck"))->m_bValue = g_bAirStuck;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Event Logger"))->m_bValue = g_bEventLog;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Auto Knife"))->m_bValue = g_bAutoKnife;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Show Fake Lag Model"))->m_bValue = g_bShowFakeLagModel;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake Lag On Key"))->m_bValue = g_bFakeLagOnKey;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake Lag Hotkey"))->m_iValue = g_kLagKey;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake Lag"))->m_iValue = g_nFakeLag;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake Lag Amount"))->m_iValue = g_iFakeLagAmount;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake Lag On Land"))->m_bValue = g_bFakelagOnLand;
	CConfig::get().GetValuePtr(XorStr("Misc"), XorStr("Fake lag only when moving"))->m_bValue = g_bFakelagOnlyOnMove;
}

void MiscVars::Setup()
{
	Crosshair.Setup();

	CConfig::get().SetValue(XorStr("Misc"), XorStr("Show Ranks"), ConfigValue(g_bShowRanks, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Bunny Hop"), ConfigValue(g_bBunnyHop, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Auto Strafe"), ConfigValue(g_bAutoStrafe, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Circle Strafe"), ConfigValue(g_bCircleStrafe, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Rage Strafe"), ConfigValue(g_bRageStrafe, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Strafe Direction"), ConfigValue(g_iStrafeDirection, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Air Crouch"), ConfigValue(g_bAirCrouch, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("FOV Changer"), ConfigValue(g_bFOVChanger, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("FOV"), ConfigValue(g_iFOVChangerValue, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Thirdperson"), ConfigValue(g_bThirdPerson, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Thirdperson Spectate"), ConfigValue(g_bThirdPersonSpectator, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Location Spammer"), ConfigValue(g_bLocationSpammer, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Location Spammer Mode"), ConfigValue(g_iLocationSpammerMode, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Report Spammer"), ConfigValue(g_bReportSpammer, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Name Stealer"), ConfigValue(g_bNameStealer, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Chat Spam"), ConfigValue(g_bChatSpammer, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Auto Accept"), ConfigValue(g_bAutoAccept, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Clan Tag"), ConfigValue(g_bClanTag, CONFIG_BOOL));
	//CConfig::get().SetValue(XorStr("Misc"), XorStr("AirStuck"), ConfigValue(g_bAirStuck, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Event Logger"), ConfigValue(g_bEventLog, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Auto Knife"), ConfigValue(g_bAutoKnife, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Show Fake Lag Model"), ConfigValue(g_bShowFakeLagModel, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Fake Lag On Key"), ConfigValue(g_bFakeLagOnKey, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Fake Lag Hotkey"), ConfigValue(g_kLagKey, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Fake Lag"), ConfigValue(g_nFakeLag, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Fake Lag Amount"), ConfigValue(g_iFakeLagAmount, CONFIG_INT));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Fake Lag On Land"), ConfigValue(g_bFakelagOnLand, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Misc"), XorStr("Fake lag only when moving"), ConfigValue(g_bFakelagOnlyOnMove, CONFIG_BOOL));
}

void WeaponConfigVars::WeaponConfig_t::LoadValues(std::string name)
{
	std::string section_name = name + XorStr(": Menu");
	bEnabled = CConfig::get().GetValuePtr(section_name, XorStr("Weapon Config Enabled"))->m_bValue;
	menu.g_kAimKey = (ButtonCode_t)CConfig::get().GetValuePtr(section_name, XorStr("Aim Key"))->m_iValue;
	menu.g_kTriggerKey = (ButtonCode_t)CConfig::get().GetValuePtr(section_name, XorStr("Trigger Key"))->m_iValue;

	section_name = name + XorStr(": Aimbot");
	legitbot.g_bAimbotEnabled = CConfig::get().GetValuePtr(section_name, XorStr("Enabled"))->m_bValue;
	legitbot.g_iAimbotStyle = CConfig::get().GetValuePtr(section_name, XorStr("Aimbot Style"))->m_iValue;
	legitbot.g_iSmoothType = CConfig::get().GetValuePtr(section_name, XorStr("Smooth Style"))->m_iValue;
	legitbot.g_bHitscanEnabled = CConfig::get().GetValuePtr(section_name, XorStr("Hitscan"))->m_bValue;
	legitbot.g_iHitbox = CConfig::get().GetValuePtr(section_name, XorStr("Hitbox"))->m_iValue;
	legitbot.g_iFOV = CConfig::get().GetValuePtr(section_name, XorStr("FOV"))->m_iValue;
	legitbot.g_flSmoothX = CConfig::get().GetValuePtr(section_name, XorStr("Smooth X"))->m_flValue;
	legitbot.g_flSmoothY = CConfig::get().GetValuePtr(section_name, XorStr("Smooth Y"))->m_flValue;
	legitbot.g_bAutoStop = CConfig::get().GetValuePtr(section_name, XorStr("Auto Stop"))->m_bValue;
	legitbot.g_bAutoDuck = CConfig::get().GetValuePtr(section_name, XorStr("Auto Duck"))->m_bValue;
	legitbot.g_bPSilent = CConfig::get().GetValuePtr(section_name, XorStr("pSilent"))->m_bValue;
	legitbot.g_bRCSEnabled = CConfig::get().GetValuePtr(section_name, XorStr("RCS"))->m_bValue;
	legitbot.g_flRCSPitchScale = CConfig::get().GetValuePtr(section_name, XorStr("RCS Pitch Scale"))->m_flValue;
	legitbot.g_flRCSYawScale = CConfig::get().GetValuePtr(section_name, XorStr("RCS Yaw Scale"))->m_flValue;
	legitbot.g_bIgnoreJumpingEnemy = CConfig::get().GetValuePtr(section_name, XorStr("Ignore Jumping Enemy"))->m_bValue;
	legitbot.g_bSilentRCS = CConfig::get().GetValuePtr(section_name, XorStr("No Visual Recoil"))->m_bValue;
	legitbot.g_bStandaloneRCS = CConfig::get().GetValuePtr(section_name, XorStr("Standalone RCS"))->m_bValue;
	legitbot.g_bSmokeCheck = CConfig::get().GetValuePtr(section_name, XorStr("Smoke Check"))->m_bValue;

	section_name = name + XorStr(": RageBot");
	ragebot.g_bAimStep = CConfig::get().GetValuePtr(section_name, XorStr("AimStep"))->m_bValue;
	ragebot.g_bAutoFire = CConfig::get().GetValuePtr(section_name, XorStr("Autofire"))->m_bValue;
	ragebot.g_iSilentAim = CConfig::get().GetValuePtr(section_name, XorStr("Silent Aim"))->m_iValue;
	ragebot.g_flPointScale = CConfig::get().GetValuePtr(section_name, XorStr("Point Scale"))->m_flValue;
	ragebot.g_bSmartHitscan = CConfig::get().GetValuePtr(section_name, XorStr("Smart Hitscan"))->m_bValue;
	ragebot.g_bHitscan = CConfig::get().GetValuePtr(section_name, XorStr("Hitscan"))->m_bValue;

	ragebot.HitgroupSelections[GROUP_HEAD] = CConfig::get().GetValuePtr(section_name, XorStr("Scan Head"))->m_bValue;
	ragebot.HitgroupSelections[GROUP_CHEST] = CConfig::get().GetValuePtr(section_name, XorStr("Scan Chest"))->m_bValue;
	ragebot.HitgroupSelections[GROUP_UPPER_ARMS] = CConfig::get().GetValuePtr(section_name, XorStr("Scan Upper Arms"))->m_bValue;
	ragebot.HitgroupSelections[GROUP_LOWER_ARMS] = CConfig::get().GetValuePtr(section_name, XorStr("Scan Lower Arms"))->m_bValue;
	ragebot.HitgroupSelections[GROUP_UPPER_LEGS] = CConfig::get().GetValuePtr(section_name, XorStr("Scan Upper Legs"))->m_bValue;
	ragebot.HitgroupSelections[GROUP_LOWER_LEGS] = CConfig::get().GetValuePtr(section_name, XorStr("Scan Lower Legs"))->m_bValue;
	ragebot.HitgroupSelections[GROUP_HANDS] = CConfig::get().GetValuePtr(section_name, XorStr("Scan Hands"))->m_bValue;
	ragebot.HitgroupSelections[GROUP_FEET] = CConfig::get().GetValuePtr(section_name, XorStr("Scan Feet"))->m_bValue;

	ragebot.g_bMultipoint = CConfig::get().GetValuePtr(section_name, XorStr("Multipoint"))->m_bValue;

	ragebot.g_iHitbox = CConfig::get().GetValuePtr(section_name, XorStr("Hitbox"))->m_iValue;
	ragebot.g_bAutoStop = CConfig::get().GetValuePtr(section_name, XorStr("Auto Stop"))->m_bValue;
	ragebot.g_bAutoStopOnKillShot = CConfig::get().GetValuePtr(section_name, XorStr("Stop Only On Killshot"))->m_bValue;
	ragebot.g_bAutoDuck = CConfig::get().GetValuePtr(section_name, XorStr("Auto Duck"))->m_bValue;
	ragebot.g_bAutoScope = CConfig::get().GetValuePtr(section_name, XorStr("Auto Scope"))->m_bValue;
	ragebot.g_bHitchance = CConfig::get().GetValuePtr(section_name, XorStr("Hitchance"))->m_bValue;
	ragebot.g_flHitchanceValue = CConfig::get().GetValuePtr(section_name, XorStr("Hitchance Percent"))->m_flValue;
	ragebot.g_bAutoWall = CConfig::get().GetValuePtr(section_name, XorStr("AutoWall"))->m_bValue;
	ragebot.g_iMinimumDamage = CConfig::get().GetValuePtr(section_name, XorStr("Minimum Damage"))->m_iValue;


	section_name = name + XorStr(": Triggerbot");
	triggerbot.g_bTriggerbotEnabled = CConfig::get().GetValuePtr(section_name, XorStr("Enabled"))->m_bValue;
	triggerbot.g_bAutoshoot = CConfig::get().GetValuePtr(section_name, XorStr("AutoShoot"))->m_bValue;
	triggerbot.g_bTriggerbotHead = CConfig::get().GetValuePtr(section_name, XorStr("Head"))->m_bValue;
	triggerbot.g_bTriggerbotChest = CConfig::get().GetValuePtr(section_name, XorStr("Chest"))->m_bValue;
	triggerbot.g_bTriggerbotStomach = CConfig::get().GetValuePtr(section_name, XorStr("Stomach"))->m_bValue;
	triggerbot.g_bTriggerbotArms = CConfig::get().GetValuePtr(section_name, XorStr("Arms"))->m_bValue;
	triggerbot.g_bTriggerbotLegs = CConfig::get().GetValuePtr(section_name, XorStr("Legs"))->m_bValue;
	triggerbot.g_bHitChance = CConfig::get().GetValuePtr(section_name, XorStr("Hitchance"))->m_bValue;
	triggerbot.g_flHitchancePercentage = CConfig::get().GetValuePtr(section_name, XorStr("Hitchance Percent"))->m_flValue;


	section_name = name + XorStr(": Misc");
	misc.g_bAutoPistol = CConfig::get().GetValuePtr(section_name, XorStr("Auto Pistol"))->m_bValue;


	section_name = name + XorStr(": Resolver");
	resolver.g_bBackTracking = CConfig::get().GetValuePtr(section_name, XorStr("Backtracking"))->m_bValue;
}

void WeaponConfigVars::WeaponConfig_t::SaveValues(std::string name)
{
	std::string section_name = name + XorStr(": Menu");
	CConfig::get().GetValuePtr(section_name, XorStr("Weapon Config Enabled"))->m_bValue = bEnabled;
	CConfig::get().GetValuePtr(section_name, XorStr("Aim Key"))->m_iValue = menu.g_kAimKey;
	CConfig::get().GetValuePtr(section_name, XorStr("Trigger Key"))->m_iValue = menu.g_kTriggerKey;

	section_name = name + XorStr(": Aimbot");
	CConfig::get().GetValuePtr(section_name, XorStr("Enabled"))->m_bValue = legitbot.g_bAimbotEnabled;
	CConfig::get().GetValuePtr(section_name, XorStr("Aimbot Style"))->m_iValue = legitbot.g_iAimbotStyle;
	CConfig::get().GetValuePtr(section_name, XorStr("Hitscan"))->m_bValue = legitbot.g_bHitscanEnabled;
	CConfig::get().GetValuePtr(section_name, XorStr("Hitbox"))->m_iValue = legitbot.g_iHitbox;
	CConfig::get().GetValuePtr(section_name, XorStr("FOV"))->m_iValue = legitbot.g_iFOV;
	CConfig::get().GetValuePtr(section_name, XorStr("Smooth Style"))->m_iValue = legitbot.g_iSmoothType;
	CConfig::get().GetValuePtr(section_name, XorStr("Smooth X"))->m_flValue = legitbot.g_flSmoothX;
	CConfig::get().GetValuePtr(section_name, XorStr("Smooth Y"))->m_flValue = legitbot.g_flSmoothY;
	CConfig::get().GetValuePtr(section_name, XorStr("Auto Stop"))->m_bValue = legitbot.g_bAutoStop;
	CConfig::get().GetValuePtr(section_name, XorStr("Auto Duck"))->m_bValue = legitbot.g_bAutoDuck;
	CConfig::get().GetValuePtr(section_name, XorStr("pSilent"))->m_bValue = legitbot.g_bPSilent;
	CConfig::get().GetValuePtr(section_name, XorStr("Ignore Jumping Enemy"))->m_bValue = legitbot.g_bIgnoreJumpingEnemy;
	CConfig::get().GetValuePtr(section_name, XorStr("RCS"))->m_bValue = legitbot.g_bRCSEnabled;
	CConfig::get().GetValuePtr(section_name, XorStr("RCS Pitch Scale"))->m_flValue = legitbot.g_flRCSPitchScale;
	CConfig::get().GetValuePtr(section_name, XorStr("RCS Yaw Scale"))->m_flValue = legitbot.g_flRCSYawScale;
	CConfig::get().GetValuePtr(section_name, XorStr("No Visual Recoil"))->m_bValue = legitbot.g_bSilentRCS;
	CConfig::get().GetValuePtr(section_name, XorStr("Standalone RCS"))->m_bValue = legitbot.g_bStandaloneRCS;
	CConfig::get().GetValuePtr(section_name, XorStr("Smoke Check"))->m_bValue = legitbot.g_bSmokeCheck;

	section_name = name + XorStr(": Resolver");
	CConfig::get().GetValuePtr(section_name, XorStr("Backtracking"))->m_bValue = resolver.g_bBackTracking;

	section_name = name + XorStr(": RageBot");
	CConfig::get().GetValuePtr(section_name, XorStr("AimStep"))->m_bValue = ragebot.g_bAimStep;
	CConfig::get().GetValuePtr(section_name, XorStr("Autofire"))->m_bValue = ragebot.g_bAutoFire;
	CConfig::get().GetValuePtr(section_name, XorStr("Silent Aim"))->m_iValue = ragebot.g_iSilentAim;
	CConfig::get().GetValuePtr(section_name, XorStr("Point Scale"))->m_flValue = ragebot.g_flPointScale;
	CConfig::get().GetValuePtr(section_name, XorStr("Smart Hitscan"))->m_bValue = ragebot.g_bSmartHitscan;
	CConfig::get().GetValuePtr(section_name, XorStr("Hitscan"))->m_bValue = ragebot.g_bHitscan;

	CConfig::get().GetValuePtr(section_name, XorStr("Scan Head"))->m_bValue = ragebot.HitgroupSelections[GROUP_HEAD];
	CConfig::get().GetValuePtr(section_name, XorStr("Scan Chest"))->m_bValue = ragebot.HitgroupSelections[GROUP_CHEST];
	CConfig::get().GetValuePtr(section_name, XorStr("Scan Upper Arms"))->m_bValue = ragebot.HitgroupSelections[GROUP_UPPER_ARMS];
	CConfig::get().GetValuePtr(section_name, XorStr("Scan Lower Arms"))->m_bValue = ragebot.HitgroupSelections[GROUP_LOWER_ARMS];
	CConfig::get().GetValuePtr(section_name, XorStr("Scan Upper Legs"))->m_bValue = ragebot.HitgroupSelections[GROUP_UPPER_LEGS];
	CConfig::get().GetValuePtr(section_name, XorStr("Scan Lower Legs"))->m_bValue = ragebot.HitgroupSelections[GROUP_LOWER_LEGS];
	CConfig::get().GetValuePtr(section_name, XorStr("Scan Hands"))->m_bValue = ragebot.HitgroupSelections[GROUP_HANDS];
	CConfig::get().GetValuePtr(section_name, XorStr("Scan Feet"))->m_bValue = ragebot.HitgroupSelections[GROUP_FEET];

	CConfig::get().GetValuePtr(section_name, XorStr("Multipoint"))->m_bValue = ragebot.g_bMultipoint;
	CConfig::get().GetValuePtr(section_name, XorStr("Hitbox"))->m_iValue = ragebot.g_iHitbox;
	CConfig::get().GetValuePtr(section_name, XorStr("Auto Stop"))->m_bValue = ragebot.g_bAutoStop;
	CConfig::get().GetValuePtr(section_name, XorStr("Stop Only On Killshot"))->m_bValue = ragebot.g_bAutoStopOnKillShot;
	CConfig::get().GetValuePtr(section_name, XorStr("Auto Duck"))->m_bValue = ragebot.g_bAutoDuck;
	CConfig::get().GetValuePtr(section_name, XorStr("Auto Scope"))->m_bValue = ragebot.g_bAutoScope;
	CConfig::get().GetValuePtr(section_name, XorStr("Hitchance"))->m_bValue = ragebot.g_bHitchance;
	CConfig::get().GetValuePtr(section_name, XorStr("Hitchance Percent"))->m_flValue = ragebot.g_flHitchanceValue;
	CConfig::get().GetValuePtr(section_name, XorStr("AutoWall"))->m_bValue = ragebot.g_bAutoWall;
	CConfig::get().GetValuePtr(section_name, XorStr("Minimum Damage"))->m_iValue = ragebot.g_iMinimumDamage;

	section_name = name + XorStr(": Triggerbot");
	CConfig::get().GetValuePtr(section_name, XorStr("Enabled"))->m_bValue = triggerbot.g_bTriggerbotEnabled;
	CConfig::get().GetValuePtr(section_name, XorStr("AutoShoot"))->m_bValue = triggerbot.g_bAutoshoot;
	CConfig::get().GetValuePtr(section_name, XorStr("Head"))->m_bValue = triggerbot.g_bTriggerbotHead;
	CConfig::get().GetValuePtr(section_name, XorStr("Chest"))->m_bValue = triggerbot.g_bTriggerbotChest;
	CConfig::get().GetValuePtr(section_name, XorStr("Stomach"))->m_bValue = triggerbot.g_bTriggerbotStomach;
	CConfig::get().GetValuePtr(section_name, XorStr("Arms"))->m_bValue = triggerbot.g_bTriggerbotArms;
	CConfig::get().GetValuePtr(section_name, XorStr("Legs"))->m_bValue = triggerbot.g_bTriggerbotLegs;
	CConfig::get().GetValuePtr(section_name, XorStr("Hitchance"))->m_bValue = triggerbot.g_bHitChance;
	CConfig::get().GetValuePtr(section_name, XorStr("Hitchance Percent"))->m_flValue = triggerbot.g_flHitchancePercentage;

	section_name = name + XorStr(": Misc");
	CConfig::get().GetValuePtr(section_name, XorStr("Auto Pistol"))->m_bValue = misc.g_bAutoPistol;
}

void WeaponConfigVars::WeaponConfig_t::Setup(std::string name)
{
	std::string section_name = name + XorStr(": Menu");
	CConfig::get().SetValue(section_name, XorStr("Weapon Config Enabled"), ConfigValue(bEnabled, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Aim Key"), ConfigValue(menu.g_kAimKey, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("Trigger Key"), ConfigValue(menu.g_kTriggerKey, CONFIG_INT));

	section_name = name + XorStr(": Aimbot");
	CConfig::get().SetValue(section_name, XorStr("Enabled"), ConfigValue(legitbot.g_bAimbotEnabled, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Aimbot Style"), ConfigValue(legitbot.g_iAimbotStyle, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("Hitscan"), ConfigValue(legitbot.g_bHitscanEnabled, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Hitbox"), ConfigValue(legitbot.g_iHitbox, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("FOV"), ConfigValue(legitbot.g_iFOV, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("Smooth Style"), ConfigValue(legitbot.g_iSmoothType, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("Smooth X"), ConfigValue(legitbot.g_flSmoothX, CONFIG_FLOAT));
	CConfig::get().SetValue(section_name, XorStr("Smooth Y"), ConfigValue(legitbot.g_flSmoothY, CONFIG_FLOAT));
	CConfig::get().SetValue(section_name, XorStr("Auto Stop"), ConfigValue(legitbot.g_bAutoStop, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Auto Duck"), ConfigValue(legitbot.g_bAutoDuck, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("pSilent"), ConfigValue(legitbot.g_bPSilent, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Ignore Jumping Enemy"), ConfigValue(legitbot.g_bIgnoreJumpingEnemy, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("RCS"), ConfigValue(legitbot.g_bRCSEnabled, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("RCS Pitch Scale"), ConfigValue(legitbot.g_flRCSPitchScale, CONFIG_FLOAT));
	CConfig::get().SetValue(section_name, XorStr("RCS Yaw Scale"), ConfigValue(legitbot.g_flRCSYawScale, CONFIG_FLOAT));
	CConfig::get().SetValue(section_name, XorStr("No Visual Recoil"), ConfigValue(legitbot.g_bSilentRCS, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Standalone RCS"), ConfigValue(legitbot.g_bStandaloneRCS, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Smoke Check"), ConfigValue(legitbot.g_bSmokeCheck, CONFIG_BOOL));

	section_name = name + XorStr(": Resolver");
	CConfig::get().SetValue(section_name, XorStr("Backtracking"), ConfigValue(resolver.g_bBackTracking, CONFIG_BOOL));

	section_name = name + XorStr(": RageBot");
	CConfig::get().SetValue(section_name, XorStr("AimStep"), ConfigValue(ragebot.g_bAimStep, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Autofire"), ConfigValue(ragebot.g_bAutoFire, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Silent Aim"), ConfigValue(ragebot.g_iSilentAim, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("Point Scale"), ConfigValue(ragebot.g_flPointScale, CONFIG_FLOAT));
	CConfig::get().SetValue(section_name, XorStr("Smart Hitscan"), ConfigValue(ragebot.g_bSmartHitscan, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Hitscan"), ConfigValue(ragebot.g_bHitscan, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Scan Head"), ConfigValue(ragebot.HitgroupSelections[GROUP_HEAD], CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Scan Chest"), ConfigValue(ragebot.HitgroupSelections[GROUP_CHEST], CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Scan Upper Arms"), ConfigValue(ragebot.HitgroupSelections[GROUP_UPPER_ARMS], CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Scan Lower Arms"), ConfigValue(ragebot.HitgroupSelections[GROUP_LOWER_ARMS], CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Scan Upper Legs"), ConfigValue(ragebot.HitgroupSelections[GROUP_UPPER_LEGS], CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Scan Lower Legs"), ConfigValue(ragebot.HitgroupSelections[GROUP_LOWER_LEGS], CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Scan Hands"), ConfigValue(ragebot.HitgroupSelections[GROUP_HANDS], CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Scan Feet"), ConfigValue(ragebot.HitgroupSelections[GROUP_FEET], CONFIG_BOOL));
	
	CConfig::get().SetValue(section_name, XorStr("Multipoint"), ConfigValue(ragebot.g_bMultipoint, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Hitbox"), ConfigValue(ragebot.g_iHitbox, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("Auto Stop"), ConfigValue(ragebot.g_bAutoStop, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Stop Only On Killshot"), ConfigValue(ragebot.g_bAutoStopOnKillShot, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Auto Duck"), ConfigValue(ragebot.g_bAutoDuck, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Auto Scope"), ConfigValue(ragebot.g_bAutoScope, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Hitchance"), ConfigValue(ragebot.g_bHitchance, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Hitchance Percent"), ConfigValue(ragebot.g_flHitchanceValue, CONFIG_FLOAT));
	CConfig::get().SetValue(section_name, XorStr("AutoWall"), ConfigValue(ragebot.g_bAutoWall, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Minimum Damage"), ConfigValue(ragebot.g_iMinimumDamage, CONFIG_INT));

	section_name = name + XorStr(": Triggerbot");
	CConfig::get().SetValue(section_name, XorStr("Enabled"), ConfigValue(triggerbot.g_bTriggerbotEnabled, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("AutoShoot"), ConfigValue(triggerbot.g_bAutoshoot, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Head"), ConfigValue(triggerbot.g_bTriggerbotHead, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Chest"), ConfigValue(triggerbot.g_bTriggerbotChest, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Stomach"), ConfigValue(triggerbot.g_bTriggerbotStomach, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Arms"), ConfigValue(triggerbot.g_bTriggerbotArms, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Legs"), ConfigValue(triggerbot.g_bTriggerbotLegs, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Hitchance"), ConfigValue(triggerbot.g_bHitChance, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("Hitchance Percent"), ConfigValue(triggerbot.g_flHitchancePercentage, CONFIG_FLOAT));

	section_name = name + XorStr(": Misc");
	CConfig::get().SetValue(section_name, XorStr("Auto Pistol"), ConfigValue(misc.g_bAutoPistol, CONFIG_BOOL));
}

void WeaponConfigVars::LoadValues()
{
	for (int i = 0; i < 65; i++)
	{
		std::string name = Utils::GetWeaponName(static_cast<EItemDefinitionIndex>(i));

		if (name == "")
			continue;

		WeaponConfig[i].LoadValues(name);
	}
}

void WeaponConfigVars::SaveValues()
{
	for (int i = 0; i < 65; i++)
	{
		std::string name = Utils::GetWeaponName(static_cast<EItemDefinitionIndex>(i));

		if (name == "")
			continue;

		WeaponConfig[i].SaveValues(name);
	}
}

void WeaponConfigVars::Setup()
{
	for (int i = 0; i < 65; i++)
	{
		std::string name = Utils::GetWeaponName(static_cast<EItemDefinitionIndex>(i));

		if (name == "")
			continue;

		WeaponConfig[i].Setup(name);
	}
}

void ThemeVars::LoadValues()
{
	
}

void ThemeVars::SaveValues()
{
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Text Color"), ConfigValue(col_Text, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Primary Color"), ConfigValue(col_Primary, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Secondary Color"), ConfigValue(col_Secondary, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Tertiary Color"), ConfigValue(col_Tertiary, CONFIG_COLOR));
}

void ThemeVars::Setup()
{
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Text Color"), ConfigValue(col_Text, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Primary Color"), ConfigValue(col_Primary, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Secondary Color"), ConfigValue(col_Secondary, CONFIG_COLOR));
	CConfig::get().SetValue(XorStr("Menu"), XorStr("Tertiary Color"), ConfigValue(col_Tertiary, CONFIG_COLOR));
}

void AAVars::AAVars_vals::SetupValues(int i) const
{
	std::string section_name = XorStr("AA ") + std::to_string(i);
	CConfig::get().SetValue(section_name, XorStr("AA Hotkey"), ConfigValue(get().g_kAAHotKey[i], CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("AA Pitch"), ConfigValue(g_iAAPitch, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("AA Fake Angle"), ConfigValue(g_bShouldFake, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("AA At Targets"), ConfigValue(g_bAtTargets, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("AA Yaw"), ConfigValue(g_iAAFakeYaw, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("AA Yaw 2"), ConfigValue(g_iAAYaw, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("AA Yaw 2 Offset"), ConfigValue(g_iAAFakeYawOffset, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("AA Yaw 1 Offset"), ConfigValue(g_iAARealYawOffset, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("AA Anti LBY"), ConfigValue(g_bAntiLBY, CONFIG_BOOL));
	CConfig::get().SetValue(section_name, XorStr("AA Anti LBY Offset"), ConfigValue(g_flAntiLBYOffset, CONFIG_FLOAT));
	CConfig::get().SetValue(section_name, XorStr("AA Yaw 1 Jitter Range"), ConfigValue(g_iJitterRangeFirst, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("AA Yaw 2 Jitter Range"), ConfigValue(g_iJitterRangeSecond, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("AA Yaw Moving"), ConfigValue(g_iMovingAA, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("AA Yaw Moving Offset"), ConfigValue(g_iMovingAAOffset, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("AA Yaw Moving Jitter Range"), ConfigValue(g_iMovingJitterRange, CONFIG_INT));
	CConfig::get().SetValue(section_name, XorStr("Freestanding"), ConfigValue(g_bFreeStanding, CONFIG_BOOL));
}

void AAVars::AAVars_vals::LoadValues(int i)
{
	std::string section_name = XorStr("AA ") + std::to_string(i);
	get().g_kAAHotKey[i] = (ButtonCode_t)CConfig::get().GetValuePtr(section_name, XorStr("AA Hotkey"))->m_iValue;
	g_iAAPitch = CConfig::get().GetValuePtr(section_name, XorStr("AA Pitch"))->m_iValue;
	g_bShouldFake = CConfig::get().GetValuePtr(section_name, XorStr("AA Fake Angle"))->m_bValue;
	g_bAtTargets = CConfig::get().GetValuePtr(section_name, XorStr("AA At Targets"))->m_bValue;
	g_iAAYaw = CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw"))->m_iValue;
	g_iAAFakeYaw = CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw 2"))->m_iValue;
	g_iAAFakeYawOffset = CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw 2 Offset"))->m_iValue;
	g_iAARealYawOffset = CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw 1 Offset"))->m_iValue;
	g_bAntiLBY = CConfig::get().GetValuePtr(section_name, XorStr("AA Anti LBY"))->m_bValue;
	g_flAntiLBYOffset = CConfig::get().GetValuePtr(section_name, XorStr("AA Anti LBY Offset"))->m_flValue;
	g_iJitterRangeFirst = CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw 1 Jitter Range"))->m_iValue;
	g_iJitterRangeSecond = CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw 2 Jitter Range"))->m_iValue;

	g_iMovingAA = CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw Moving"))->m_iValue;
	g_iMovingAAOffset = CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw Moving Offset"))->m_iValue;
	g_iMovingJitterRange = CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw Moving Jitter Range"))->m_iValue;

	g_bFreeStanding = CConfig::get().GetValuePtr(section_name, XorStr("Freestanding"))->m_bValue;
}

void AAVars::AAVars_vals::SaveValues(int i) const
{
	std::string section_name = XorStr("AA ") + std::to_string(i);
	CConfig::get().GetValuePtr(section_name, XorStr("AA Hotkey"))->m_iValue = get().g_kAAHotKey[i];
	CConfig::get().GetValuePtr(section_name, XorStr("AA Pitch"))->m_iValue = get().aa[i].g_iAAPitch;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Fake Angle"))->m_bValue = get().aa[i].g_bShouldFake;
	CConfig::get().GetValuePtr(section_name, XorStr("AA At Targets"))->m_bValue = get().aa[i].g_bAtTargets;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw"))->m_iValue = get().aa[i].g_iAAYaw;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw 2"))->m_iValue = get().aa[i].g_iAAFakeYaw;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw 2 Offset"))->m_iValue = get().aa[i].g_iAAFakeYawOffset;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw 1 Offset"))->m_iValue = get().aa[i].g_iAARealYawOffset;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Anti LBY"))->m_bValue = get().aa[i].g_bAntiLBY;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Anti LBY Offset"))->m_flValue = get().aa[i].g_flAntiLBYOffset;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw 1 Jitter Range"))->m_iValue = get().aa[i].g_iJitterRangeFirst;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw 2 Jitter Range"))->m_iValue = get().aa[i].g_iJitterRangeSecond;
	CConfig::get().GetValuePtr(section_name, XorStr("Freestanding"))->m_bValue = get().aa[i].g_bFreeStanding;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw Moving"))->m_iValue = get().aa[i].g_iMovingAA;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw Moving Offset"))->m_iValue = get().aa[i].g_iMovingAAOffset;
	CConfig::get().GetValuePtr(section_name, XorStr("AA Yaw Moving Jitter Range"))->m_iValue = get().aa[i].g_iMovingJitterRange;
}

void AAVars::LoadValues()
{
	get().g_bAAEnabled = CConfig::get().GetValuePtr(XorStr("Anti-Aim"), XorStr("Anti-Aim Enabled"))->m_bValue;

	get().g_bShowFakeAngle = CConfig::get().GetValuePtr(XorStr("Anti-Aim"), XorStr("Fake Angle Model"))->m_bValue;
	get().g_bShowLBYAngle = CConfig::get().GetValuePtr(XorStr("Anti-Aim"), XorStr("LBY Angle Model"))->m_bValue;

	get().g_bAALinesEnabled = CConfig::get().GetValuePtr(XorStr("Anti-Aim"), XorStr("Show AA Lines"))->m_bValue;

	for (int i = 0; i < 4; i++)
		aa[i].LoadValues(i);
}

void AAVars::SaveValues() const
{
	CConfig::get().GetValuePtr(XorStr("Anti-Aim"), XorStr("Anti-Aim Enabled"))->m_bValue = get().g_bAAEnabled;
	CConfig::get().GetValuePtr(XorStr("Anti-Aim"), XorStr("Fake Angle Model"))->m_bValue = get().g_bShowFakeAngle;
	CConfig::get().GetValuePtr(XorStr("Anti-Aim"), XorStr("LBY Angle Model"))->m_bValue = get().g_bShowLBYAngle;
	CConfig::get().GetValuePtr(XorStr("Anti-Aim"), XorStr("Show AA Lines"))->m_bValue = get().g_bAALinesEnabled;

	for (int i = 0; i < 4; i++)
		aa[i].SaveValues(i);
}

void AAVars::Setup() const
{
	CConfig::get().SetValue(XorStr("Anti-Aim"), XorStr("Anti-Aim Enabled"), ConfigValue(get().g_bAAEnabled, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Anti-Aim"), XorStr("Fake Angle Model"), ConfigValue(get().g_bShowFakeAngle, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Anti-Aim"), XorStr("LBY Angle Model"), ConfigValue(get().g_bShowLBYAngle, CONFIG_BOOL));
	CConfig::get().SetValue(XorStr("Anti-Aim"), XorStr("Show AA Lines"), ConfigValue(get().g_bAALinesEnabled, CONFIG_BOOL));

	for (int i = 0; i < 4; i++)
		aa[i].SetupValues(i);
}
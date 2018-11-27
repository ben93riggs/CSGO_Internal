#pragma once
#include "SourceEngine/Definitions.hpp"

class DrawManager;

enum TABS
{
	//removed
};

class AimbotTab
{
	void DrawRCS() const;
	void DrawFakeLag() const;
	void DrawHitscan() const;
	void DrawNospread() const;
	void DrawSpread() const;
	void DrawLegit() const;
	void DrawAACreator() const;
	void DrawResolver() const;

public:
	void Think() const;
};

class PlayerListTab
{
public:
	void Think() const;
};

class VisualsTab
{
	void DrawChams() const;
	static void DrawESP();
	static void DrawGlow();
	void DrawWorldModifiers() const;
	static void DrawMisc();
	static void DrawRemovals();
public:
	
	void Think() const;
	void DrawCrosshair() const;
};

class MiscTab
{
public:
	
	void Think() const;
};

class SkinChangerTab
{

public:
	void SkinChanger() const;
	void Think() const;
};

class ConfigTab
{
	static void DrawThemeChanger();
	void DrawConfig() const;
	void DrawWeaponConfigs() const;

public:
	void Think() const;
	static void ThemeChanger();
};

class Menu
{
	AimbotTab aim;
	PlayerListTab plist;
	VisualsTab vistab;
	MiscTab misctab;
	SkinChangerTab skintab;
	ConfigTab configtab;

	bool FontSet = false;

	void MainMenu() const;
	static void DebugWindow();
	static void Cleanup();
	void WeaponConfigChanger() const;

public:
	ImFont * defaultfont;
	ImFont* symbolfont;
	void GUI();
};

extern Menu menu;
#pragma once
#include <vector>
#include "Vars.h"

#define SAVECONFIG(config_name) CreateThread(nullptr, 0, [](PVOID data) -> DWORD { CConfig::get().SaveConfig(config_name); return 0; }, nullptr, 0, nullptr);
#define LOADCONFIG(config_name) CreateThread(nullptr, 0, [](PVOID data) -> DWORD { CConfig::get().LoadConfig(config_name); return 0; }, nullptr, 0, nullptr);
#define SETUPCONFIG() CreateThread(nullptr, 0, [](PVOID data) -> DWORD { CConfig::get().Setup(); return 0; }, nullptr, 0, nullptr);

enum ConfigValueType
{
	CONFIG_COLOR,
	CONFIG_INT,
	CONFIG_FLOAT,
	CONFIG_BOOL,
	CONFIG_STRING
};

struct ConfigValue
{
	ConfigValue() : m_iValue(0), m_flValue(0), m_bValue(false), m_Type(CONFIG_INT)
	{
	}

	ConfigValue(int iValue, ConfigValueType type): m_flValue(iValue)
	{
		m_iValue = iValue;
		m_bValue = !!iValue;
		m_Type = type;
	}

	ConfigValue(float flValue, ConfigValueType type): m_iValue(flValue), m_bValue(false)
	{
		m_flValue = flValue;
		m_Type = type;
	}

	ConfigValue(bool bValue, ConfigValueType type): m_flValue(0)
	{
		m_bValue = bValue;
		m_iValue = bValue;
		m_Type = type;
	}

	ConfigValue(std::string strValue, ConfigValueType type): m_iValue(0), m_flValue(0), m_bValue(false)
	{
		m_strValue = strValue;
		m_Type = type;
	}

	ConfigValue(float cValue[4], ConfigValueType type): m_iValue(0), m_flValue(0), m_bValue(false)
	{
		m_colorValue[0] = cValue[0];
		m_colorValue[1] = cValue[1];
		m_colorValue[2] = cValue[2];
		m_colorValue[3] = cValue[3];
		m_Type = type;
	}

	int m_iValue;
	float m_flValue;
	bool m_bValue;
	std::string m_strValue;
	float m_colorValue[4];

	ConfigValueType m_Type;
};

struct ConfigVariable
{
	ConfigVariable() {}

	ConfigVariable(std::string strName, ConfigValue nValue)
	{
		m_strName = strName;
		m_nValue = nValue;
	}

	std::string m_strName;
	ConfigValue m_nValue;
};

struct ConfigGroup
{
	ConfigGroup() {}

	ConfigGroup(std::string strName)
	{
		m_strName = strName;
	}

	std::string m_strName;
	std::vector<ConfigVariable*> m_Variables;
};

class CConfig : public singleton<CConfig>
{
public:
	static void LoadColorFromConfig(float to[4], float from[4]);
	static void LoadValues();
	static void SaveValues();
	void Setup();
	void SetValue(std::string strGroup, std::string strValue, ConfigValue nValue);
	ConfigValue GetValue(std::string strGroup, std::string strValue);
	ConfigValue* GetValuePtr(std::string strGroup, std::string strValue);
	void SaveConfig(std::string strName);
	void LoadConfig(std::string strName);
	void DumpConfig();
	static std::vector<std::string> GetConfigs();
private:
	std::vector<ConfigGroup*> groups;
};
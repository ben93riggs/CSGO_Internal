#pragma once
#include "Config.h"
#include <shlobj.h>
#include <string>
#include <iostream>
#include <fstream>
#include "Globals.h"
#include <stdio.h>
#include "dirent.h"
#include "DeveloperMessages.h"
#include <future>

bool file_exists(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

void CConfig::LoadColorFromConfig(float to[4], float from[4])
{	
	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
	to[3] = from[3];
}

void CConfig::LoadValues()
{
	MenuVars::get().LoadValues();
	VisualVars::get().LoadValues();
	AAVars::get().LoadValues();
	MiscVars::get().LoadValues();
	ThemeVars::get().LoadValues();
	WeaponConfigVars::get().LoadValues();
}

void CConfig::SaveValues()
{
	MenuVars::get().SaveValues();
	VisualVars::get().SaveValues();
	AAVars::get().SaveValues();
	MiscVars::get().SaveValues();
	ThemeVars::get().SaveValues();
	WeaponConfigVars::get().SaveValues();
}

void CConfig::Setup()
{
	MenuVars::get().Setup();
	VisualVars::get().Setup();
	AAVars::get().Setup();
	MiscVars::get().Setup();
	ThemeVars::get().Setup();
	WeaponConfigVars::get().Setup();
	
	if (file_exists((std::string(getenv(XorStr("APPDATA"))) + XorStr("\\CSGO_Internal\\") + XorStr("default.cfg")).c_str()))
	{
		LoadConfig(XorStr("default"));
		SaveConfig(XorStr("default"));
	}
	else
		SAVECONFIG(XorStr("default"));
}

void CConfig::SetValue(std::string strGroup, std::string strVariable, ConfigValue nValue)
{
	bool bFoundGroup = false, bFoundVariable = false;
	for (auto pGroup : groups)
	{
		if (pGroup->m_strName == strGroup)
		{
			bFoundGroup = true;
			for (auto pVariable : pGroup->m_Variables)
			{
				if (pVariable->m_strName == strVariable)
				{
					bFoundVariable = true;
					pVariable->m_nValue = nValue;
				}
			}

			if (!bFoundVariable)
			{
				ConfigVariable* pVariable = new ConfigVariable(strVariable, nValue);
				pGroup->m_Variables.push_back(pVariable);
			}
		}

		if (bFoundVariable)
			break;
	}

	if (!bFoundGroup)
	{
		ConfigGroup* pGroup = new ConfigGroup(strGroup);

		ConfigVariable* pVariable = new ConfigVariable(strVariable, nValue);
		pGroup->m_Variables.push_back(pVariable);

		groups.push_back(pGroup);
	}
}

ConfigValue CConfig::GetValue(std::string strGroup, std::string strVariable)
{
	return *GetValuePtr(strGroup, strVariable);
}

ConfigValue* CConfig::GetValuePtr(std::string strGroup, std::string strVariable)
{
	for (auto pGroup : groups)
	{
		if (pGroup->m_strName == strGroup)
		{
			for (auto pVariable : pGroup->m_Variables)
			{
				if (pVariable->m_strName == strVariable)
				{
					return &pVariable->m_nValue;
				}
			}
		}
	}

	std::cout << strGroup << XorStr("\t\t") << strVariable << std::endl;

	return new ConfigValue();
}

bool to_bool(std::string str) {
	transform(str.begin(), str.end(), str.begin(), tolower);
	std::istringstream is(str);
	bool b;
	is >> std::boolalpha >> b;
	return b;
}

std::vector<std::string> tokenize(const std::string & str, const std::string & delimiters)
{
	std::vector<std::string> tokens;

	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}

void CConfig::SaveConfig(std::string strName)
{
	SaveValues();

	std::string szFile = (std::string(getenv(XorStr("APPDATA"))) + XorStr("\\CSGO_Internal\\") + strName + XorStr(".cfg"));

	for (auto pGroup : groups)
	{
		for (auto pVariable : pGroup->m_Variables)
		{
			switch (pVariable->m_nValue.m_Type)
			{

			case CONFIG_COLOR:
			{
				std::string tempStr = std::to_string(pVariable->m_nValue.m_colorValue[0]) + XorStr("\t") + std::to_string(pVariable->m_nValue.m_colorValue[1]) + XorStr("\t") + std::to_string(pVariable->m_nValue.m_colorValue[2]) + XorStr("\t") + std::to_string(pVariable->m_nValue.m_colorValue[3]);
				WritePrivateProfileStringA(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), tempStr.c_str(), szFile.c_str());
				break;
			}
			case CONFIG_INT:
				WritePrivateProfileStringA(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), std::to_string(pVariable->m_nValue.m_iValue).c_str(), szFile.c_str());
				break;
			case CONFIG_FLOAT:
				WritePrivateProfileStringA(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), std::to_string(pVariable->m_nValue.m_flValue).c_str(), szFile.c_str());
				break;
			case CONFIG_BOOL:
				WritePrivateProfileStringA(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), pVariable->m_nValue.m_bValue ? XorStr("true") : XorStr("false"), szFile.c_str());
				break;
			case CONFIG_STRING:
				WritePrivateProfileStringA(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), pVariable->m_nValue.m_strValue.c_str(), szFile.c_str());
				break;
			default: break;
			}
		}
	}

	DevMsg::get().OutputMessage(Color::MediumPurple(), std::string(XorStr("saved config: ") + strName).c_str());
}

void CConfig::LoadConfig(std::string strName)
{
	std::string szFile = std::string(getenv(XorStr("APPDATA"))) + XorStr("\\CSGO_Internal\\") + strName + XorStr(".cfg");

	if (!file_exists(szFile.c_str()))
	{
		Setup();
		return;
	}

	std::string tempStr;
	std::vector<std::string> strings;

	static char szValue[26];

	for (auto pGroup : groups)
	{
		for (auto pVariable : pGroup->m_Variables)
		{
			int i = 0;
			switch (pVariable->m_nValue.m_Type)
			{
			case CONFIG_COLOR:
				GetPrivateProfileStringA(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), "", szValue, 26, szFile.c_str());

				strings = tokenize(szValue, XorStr("\t"));
				for (std::string curStr : strings)
				{
					if (i >= 0 && i < 4)
					{
						pVariable->m_nValue.m_colorValue[i] = atof(curStr.c_str());
						i++;
					}
				}
				break;
			case CONFIG_INT:
				GetPrivateProfileStringA(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), "", szValue, 26, szFile.c_str());
				pVariable->m_nValue.m_iValue = atoi(szValue);
				break;
			case CONFIG_FLOAT:
				GetPrivateProfileStringA(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), "", szValue, 26, szFile.c_str());
				if (atof(szValue))
					pVariable->m_nValue.m_flValue = atof(szValue);
				break;
			case CONFIG_BOOL:
				GetPrivateProfileStringA(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), "", szValue, 26, szFile.c_str());
				
				if (!strcmp(szValue, XorStr("true")))
				{
					pVariable->m_nValue.m_iValue = 1;
					pVariable->m_nValue.m_bValue = true;
				}
				else if (!strcmp(szValue, XorStr("false")))
				{
					pVariable->m_nValue.m_iValue = 0;
					pVariable->m_nValue.m_bValue = false;
				}
				else
				{
					pVariable->m_nValue.m_iValue = GetValuePtr(pGroup->m_strName.c_str(), pVariable->m_strName.c_str())->m_iValue;
					pVariable->m_nValue.m_bValue = GetValuePtr(pGroup->m_strName.c_str(), pVariable->m_strName.c_str())->m_bValue;
				}
				break;
			case CONFIG_STRING:
				GetPrivateProfileStringA(pGroup->m_strName.c_str(), pVariable->m_strName.c_str(), "", szValue, 26, szFile.c_str());
				pVariable->m_nValue.m_strValue = szValue;
				break;
			default: break;
			}
		}
	}

	LoadValues();

	DevMsg::get().OutputMessage(Color::MediumPurple(), std::string(XorStr("loaded config: ") + strName).c_str());
}

void CConfig::DumpConfig()
{
	std::cout << XorStr("Config Dump:") << std::endl;
	for (auto pGroup : groups)
	{
		for (auto pVariable : pGroup->m_Variables)
		{
			switch (pVariable->m_nValue.m_Type)
			{
			case CONFIG_INT:
				printf("   %s -> %s -> %d\n",
					pGroup->m_strName.c_str(),
					pVariable->m_strName.c_str(),
					pVariable->m_nValue.m_iValue
				);
				break;
			case CONFIG_FLOAT:
				printf("   %s -> %s -> %g\n",
					pGroup->m_strName.c_str(),
					pVariable->m_strName.c_str(),
					pVariable->m_nValue.m_flValue
				);
				break;
			case CONFIG_BOOL:
				printf("   %s -> %s -> %s\n",
					pGroup->m_strName.c_str(),
					pVariable->m_strName.c_str(),
					pVariable->m_nValue.m_bValue ? "true" : "false"
				);
				break;
			case CONFIG_STRING:
				printf("   %s -> %s -> %s\n",
					pGroup->m_strName.c_str(),
					pVariable->m_strName.c_str(),
					pVariable->m_nValue.m_strValue.c_str()
				);
				break;
			case CONFIG_COLOR:
			{
				std::string tempStr = std::to_string(pVariable->m_nValue.m_colorValue[0]) + "\t" + std::to_string(pVariable->m_nValue.m_colorValue[1]) + "\t" + std::to_string(pVariable->m_nValue.m_colorValue[2]) + "\t" + std::to_string(pVariable->m_nValue.m_colorValue[3]);
				printf("   %s -> %s -> %s\n",
					pGroup->m_strName.c_str(),
					pVariable->m_strName.c_str(),
					tempStr.c_str()
				);
				break;
			}
			default: break;
			}
		}
	}
}

std::vector<std::string> CConfig::GetConfigs()
{
	std::vector<std::string> configNames;

	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir((std::string(getenv(XorStr("APPDATA"))) + XorStr("\\CSGO_Internal\\")).c_str())) != nullptr)
	{
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != nullptr)
		{
			std::string filename(ent->d_name);
			if (filename.find(XorStr(".cfg")) != std::string::npos)
			{
				configNames.push_back(ent->d_name);
			}
		}
		closedir(dir);
	}
	else
	{
		std::cout << opendir((std::string(getenv(XorStr("APPDATA"))) + XorStr("\\CSGO_Internal\\")).c_str()) << std::endl;
	}

	return configNames;
}
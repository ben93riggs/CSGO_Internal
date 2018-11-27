#pragma once
#include "../NetvarManager.hpp"
#include "../XorStr.hpp"

class CSGameRulesProxy
{
	template<class T>
	T GetFieldValue(int offset)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<DWORD>(this) + offset);
	}

	template<class T>
	T* GetFieldPointer(int offset)
	{
		return reinterpret_cast<T*>(reinterpret_cast<DWORD>(this) + offset);
	}

public:

	int GetRoundTime()
	{
		static DWORD roundTimeOff;

		if (!roundTimeOff)
			roundTimeOff = GET_NETVAR(XorStr("DT_CSGameRulesProxy"), XorStr("cs_gamerules_data"), XorStr("m_iRoundTime"));

		return GetFieldValue<int>(roundTimeOff);
	}

	bool IsValveDS()
	{
		static DWORD isValveDSOff;
		if (!isValveDSOff)
			isValveDSOff = GET_NETVAR(XorStr("DT_CSGameRulesProxy"), XorStr("cs_gamerules_data"), XorStr("m_bIsValveDS"));

		return GetFieldValue<bool>(isValveDSOff);
	}

	bool IsBombDropped()
	{
		static DWORD isBombDroppedOff;
		if (!isBombDroppedOff)
			isBombDroppedOff = GET_NETVAR(XorStr("DT_CSGameRulesProxy"), XorStr("cs_gamerules_data"), XorStr("m_bBombDropped"));

		return GetFieldValue<bool>(isBombDroppedOff);
	}

	bool IsBombPlanted()
	{
		static DWORD isBombPlantedOff;
		if (!isBombPlantedOff)
			isBombPlantedOff = GET_NETVAR(XorStr("DT_CSGameRulesProxy"), XorStr("cs_gamerules_data"), XorStr("m_bBombPlanted"));

		return GetFieldValue<bool>(isBombPlantedOff);
	}

};

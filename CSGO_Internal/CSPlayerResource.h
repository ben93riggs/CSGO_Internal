#pragma once
#include <assert.h>

class CSPlayerResource
{
	template<class T>
	T GetFieldValue(int offset)
	{
		return *(T*)((DWORD)this + offset);
	}

	template<class T>
	T* GetFieldPointer(int offset)
	{
		return (T*)((DWORD)this + offset);
	}

public:
	int GetPing(int index)
	{
		static auto ping_offset = 0xAE4;//GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPing")); //crashing finding netvar manually...

		if (!ping_offset)
			return 0;

		return *(int*)(this + ping_offset + index * 4);
	}

	/*int GetKills(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}*/

	/*int GetAssists(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetDeaths(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}*/

	/*bool GetConnected(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetTeam(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetPendingTeam(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	bool GetAlive(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetHealth(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}*/

	int GetPlayerC4()
	{
		static DWORD c4Off;

		if (!c4Off)
			c4Off = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return GetFieldValue<int>(c4Off);
	}

	/*int GetMVPs(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetArmor(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetScore(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetCompetitiveRanking(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetCompetitiveWins(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetCompTeammateColor(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	const char* GetClan(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return (const char*)((uintptr_t)this + offsets.DT_CSPlayerResource.m_szClan + index * 16);
	}

	int GetActiveCoinRank(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetMusicID(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetPersonaDataPublicCommendsLeader(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetPersonaDataPublicCommendsTeacher(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}

	int GetPersonaDataPublicCommendsFriendly(int index)
	{
		static DWORD killsOff;

		if (!killsOff)
			killsOff = GET_NETVAR(XorStr("DT_CSPlayerResource"), XorStr("m_iPlayerC4"));

		return *(int*)((uintptr_t)this + killsOff + index * 4);
	}*/
};

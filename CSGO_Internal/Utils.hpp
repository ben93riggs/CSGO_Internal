#pragma once
#include <Windows.h>
#include <Psapi.h>
#include "SourceEngine/SDK.hpp"
#include "SourceEngine/IClientEntity.hpp"
#include <assert.h>
#include "C_CSPlayer.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <locale>
#include <codecvt>
#include <numeric>
#include "Math.hpp"

#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

#define FindSignature FindPattern

namespace Utils
{
	inline bool is_panorama() 
	{
		static bool panorama = strstr(GetCommandLineA(), XorStr("-panorama"));
		return panorama;
	}

	inline std::string get_client_dll() 
	{
		static std::string ret = (is_panorama() ? XorStr("client_panorama.dll") : XorStr("client.dll"));
		return ret;
	}

	inline std::wstring MultiByteToWideString(const char *mbString)
	{
		// see [url]http://stackoverflow.com/questions/8032080/how-to-convert-char-to-wchar-t[/url]
		std::size_t length = std::strlen(mbString);
		std::wstring wideString(length, L'#');
		std::mbstowcs(&wideString[0], mbString, length);
		return wideString;
	}

	inline int WaitForModules(std::int32_t timeout, const std::initializer_list<std::string>& modules)
	{
		bool signaled[32] = { 0 };
		bool success = false;

		std::uint32_t totalSlept = 0;

		if (timeout == 0) {
			for (auto& mod : modules) {
				if (GetModuleHandleA(std::data(mod)) == NULL)
					return WAIT_TIMEOUT;
			}
			return WAIT_OBJECT_0;
		}

		if (timeout < 0)
			timeout = INT32_MAX;

		while (true) {
			for (auto i = 0u; i < modules.size(); ++i) {
				auto& module = *(modules.begin() + i);
				if (!signaled[i] && GetModuleHandleA(std::data(module)) != NULL) {
					signaled[i] = true;

					//
					// Checks if all modules are signaled
					//
					bool done = true;
					for (auto j = 0u; j < modules.size(); ++j) {
						if (!signaled[j]) {
							done = false;
							break;
						}
					}
					if (done) {
						success = true;
						goto exit;
					}
				}
			}
			if (totalSlept > std::uint32_t(timeout)) {
				break;
			}
			Sleep(10);
			totalSlept += 10;
		}

	exit:
		return success ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
	}

	template<class t = uintptr_t>
	t Deref(uintptr_t addr) {
		return *(t*)(addr);
	}

	template<class t = uintptr_t>
	t Follow32(uintptr_t addr) {
		return (t)(addr + 4 + Deref(addr));
	}

	inline Vector GetHitboxPosition(C_CSPlayer* pEntity, CSGOHitboxID hitbox_id)
	{
		if (pEntity)
		{
			matrix3x4_t matrix[MAXSTUDIOBONES];
			if (pEntity->SetupBones(matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0))
			{
				studiohdr_t* hdr = MODELINFO->GetStudioModel(pEntity->GetModel());
				if (hdr) 
				{
					mstudiobbox_t* hitbox = hdr->GetHitboxSet(pEntity->GetPlayerHitboxSet())->pHitbox(hitbox_id);
					if (hitbox)
					{
						Vector vMin, vMax;
						Math::VectorTransform(hitbox->bbmin, matrix[hitbox->bone], vMin);
						Math::VectorTransform(hitbox->bbmax, matrix[hitbox->bone], vMax);
						return (vMin + vMax) * 0.5f;
					}
				}
			}
		}

		return {};
	}

	inline bool bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask)
	{
		for (; *szMask; ++szMask, ++Mask, ++Data)
		{
			if (*szMask == 'x' && *Mask != *Data)
			{
				return false;
			}
		}
		return (*szMask) == 0;
	}

	inline HMODULE WaitGetModuleHandle(std::string lpModuleName)
	{
		HMODULE ModuleHandle = NULL;
		while (!ModuleHandle)
		{
			ModuleHandle = GetModuleHandleA(lpModuleName.c_str());
			if (!ModuleHandle)
				Sleep(50);
		}

		return ModuleHandle;
	}

	inline DWORD FindPatternEx(std::string moduleName, BYTE* Mask, char* szMask)
	{
		//CREDITS: learn_more

		DWORD dwAddress = (DWORD)WaitGetModuleHandle(moduleName.c_str());

		MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)dwAddress, &miModInfo, sizeof(MODULEINFO));
		DWORD dwLength = miModInfo.SizeOfImage;
		for (DWORD i = 0; i < dwLength; i++)
		{
			if (bCompare((BYTE*)(dwAddress + i), Mask, szMask))
			{
				return (DWORD)(dwAddress + i);
			}
		}
		return 0;
	}

	inline uint64_t FindPatternEx(std::string szModule, std::string szSignature)
	{
		//CREDITS: learn_more

		MODULEINFO modInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule.c_str()), &modInfo, sizeof(MODULEINFO));
		DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;
		DWORD endAddress = startAddress + modInfo.SizeOfImage;
		const char* pat = szSignature.c_str();
		DWORD firstMatch = 0;
		for (DWORD pCur = startAddress; pCur < endAddress; pCur++) {
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;    //one ?
			}
			else {
				pat = szSignature.c_str();
				firstMatch = 0;
			}
		}
		return NULL;
	}

	inline DWORD FindPattern(std::string moduleName, BYTE* Mask, char* szMask)
	{
		std::string mask_string = (char*)Mask;

		DWORD temp = FindPatternEx(moduleName, Mask, szMask);
		
		if (!temp)
		{
			std::string path = std::string(getenv(XorStr("APPDATA"))) + XorStr("\\CSGO_Internal\\");
			std::string logpath = path + XorStr("logs\\");
			std::ofstream fileout(logpath + XorStr("badsigs.txt"));
			std::string strtemp = XorStr("Bad Signature") + std::string(mask_string);
			fileout.write(strtemp.c_str(), strtemp.length());
			std::cout << mask_string << "\t\t" << szMask << std::endl;
		}
		//assert(temp);
		return temp;
	}

	inline uint64_t FindPattern(std::string szModule, std::string szSignature)
	{
		uint64_t temp = FindPatternEx(szModule, szSignature);
		if (!temp)
		{
			std::string path = std::string(getenv(XorStr("APPDATA"))) + XorStr("\\CSGO_Internal\\");
			std::string logpath = path + XorStr("logs\\");
			std::ofstream fileout(logpath + XorStr("badsigs.txt"));
			std::string strtemp = XorStr("Bad Signature: ") + std::string(szSignature);
			fileout.write(strtemp.c_str(), strtemp.length());
			fileout.close();
			printf("%s\n", szSignature.c_str());
		}
		return temp;
	}

	inline bool SigScanCompare(const char *sig, void *_addr)
	{
		const char *addr = static_cast<const char *>(_addr);

		while (true)
		{
			if (*sig == 0)
				return true;
			if (*sig != '?' && *addr != *sig)
				break;
			addr++;
			sig++;
		}

		return false;
	}

	inline void* SigScan(const char *sig, void *_base)
	{
		char *base = static_cast<char *>(_base);
		while (true)
		{
			const char *cursig = sig;
			char *curbase = base;
			while (true)
			{
				if (*cursig == 0)
					return base;
				if (*cursig != '?' && *cursig != *curbase)
					break;
				cursig++;
				curbase++;
			}

			base++;
		}
	}

	inline void StrafeCorrection(CUserCmd* cmd, Vector& qaView, Vector qaAim)
	{
		FLOAT flMove[3];
		flMove[0] = cmd->forwardmove;
		flMove[1] = cmd->sidemove;
		flMove[2] = cmd->upmove;

		Vector vMove(cmd->forwardmove, cmd->sidemove, cmd->upmove);

		QAngle flMoveNorm;
		Math::VectorAngles(flMove, flMoveNorm);

		Vector vMoveNorm(flMoveNorm[0], flMoveNorm[1], flMoveNorm[2]);

		Vector vAdjusted = vMoveNorm + (qaAim - qaView);

		FLOAT flAdjusted[3];
		flAdjusted[0] = vAdjusted.x;
		flAdjusted[1] = vAdjusted.y;
		flAdjusted[2] = vAdjusted.z;

		Vector flForward;
		Vector flSide;
		Vector flUp;

		Math::AngleVectors(flAdjusted, flForward, flSide, flUp);

		Vector vForward(flForward[0], flForward[1], flForward[2]);
		Vector vSet = vForward * vMove.Length();

		cmd->forwardmove = vSet.x;
		cmd->sidemove = vSet.y;
	}

	inline bool IsCodePtr(void* ptr)
	{
		constexpr const DWORD protect_flags = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

		MEMORY_BASIC_INFORMATION out;
		VirtualQuery(ptr, &out, sizeof out);

		return out.Type
			&& !(out.Protect & (PAGE_GUARD | PAGE_NOACCESS))
			&& out.Protect & protect_flags;
	}
#define max(a,b)            (((a) > (b)) ? (a) : (b))

	inline std::string GetWeaponName(EItemDefinitionIndex n)
	{
		switch (n)
		{
		case weapon_none:
			return XorStr("default");
		case weapon_deagle:
			return XorStr("deagle");
		case weapon_elite:
			return XorStr("elite");
		case weapon_fiveseven:
			return XorStr("fiveseven");
		case weapon_glock:
			return XorStr("glock");
		case weapon_ak47:
			return XorStr("ak47");
		case weapon_aug:
			return XorStr("aug");
		case weapon_awp:
			return XorStr("awp");
		case weapon_famas:
			return XorStr("famas");
		case weapon_g3sg1:
			return XorStr("g3sg1");
		case weapon_galilar:
			return XorStr("galilar");
		case weapon_m249:
			return XorStr("m249");
		case weapon_m4a4:
			return XorStr("m4a4");
		case weapon_mac10:
			return XorStr("mac10");
		case weapon_p90:
			return XorStr("p90");
		case weapon_ump:
			return XorStr("ump");
		case weapon_xm1014:
			return XorStr("xm1014");
		case weapon_bizon:
			return XorStr("bizon");
		case weapon_mag7:
			return XorStr("mag7");
		case weapon_negev:
			return XorStr("negev");
		case weapon_sawedoff:
			return XorStr("sawedoff");
		case weapon_tec9:
			return XorStr("tec9");
		case weapon_taser:
			return XorStr("taser");
		case weapon_hkp2000:
			return XorStr("hkp2000");
		case weapon_mp7:
			return XorStr("mp7");
		case weapon_mp9:
			return XorStr("mp9");
		case weapon_nova:
			return XorStr("nova");
		case weapon_p250:
			return XorStr("p250");
		case weapon_scar20:
			return XorStr("scar20");
		case weapon_sg553:
			return XorStr("sg553");
		case weapon_ssg08:
			return XorStr("ssg08");
		case weapon_m4a1_silencer:
			return XorStr("m4a1_silencer");
		case weapon_usp_silencer:
			return XorStr("usp_silencer");
		case weapon_cz75a:
			return XorStr("cz75a");
		case weapon_revolver:
			return XorStr("revolver");

		default:
			return XorStr("");
		}
	}

	//for playerlist only
	inline bool IsValidEnemyForPList(C_CSPlayer* ent, C_CSPlayer* pLocal)
	{
		if (!ent)
			return false;
		if (ent == pLocal)
			return false;
		ClientClass* ent_client_class = ent->GetClientClass();
		if (!ent_client_class)
			return false;
		if (ent_client_class->m_ClassID != CCSPlayer)
			return false;
		/*if (ent->GetTeam() == pLocal->GetTeam())
			return false;*/
		if (ent->GetTeam() != 2 && ent->GetTeam() != 3)
			return false;
		if (strstr(ent->sGetName().c_str(), XorStr("GOTV")))
			return false;

		return true;
	}

#pragma region stringtools
	inline const char* bool_cast(const bool b)
	{
		return b ? XorStr("true") : XorStr("false");
	}

	inline bool FClassnameIs(C_CSPlayer *pEntity, const char *szClassname)
	{
		if (!pEntity)
			return false;

		ClientClass* ent_client_class = pEntity->GetClientClass();

		if (!ent_client_class)
			return false;

		return !strcmp(ent_client_class->m_pNetworkName, szClassname) ? true : false;
	}

	inline void StdReplaceStr(std::string& replaceIn, const std::string& replace, const std::string& replaceWith)
	{
		size_t const span = replace.size();
		size_t const step = replaceWith.size();
		size_t index = 0;

		while (true)
		{
			index = replaceIn.find(replace, index);

			if (index == std::string::npos)
				break;

			replaceIn.replace(index, span, replaceWith);
			index += step;
		}
	}

	inline std::string ws2s(std::wstring wstr)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

		try
		{
			return converter.to_bytes(wstr);
		}
		catch (std::range_error)
		{
			std::stringstream s;
			s << wstr.c_str();
			return s.str();
		}
	}

	inline std::wstring s2ws(std::string str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

		try
		{
			return converter.from_bytes(str);
		}
		catch (std::range_error)
		{
			std::wostringstream s;
			s << str.c_str();
			return s.str();
		}
	}
#pragma endregion
	
	inline void SetName(const char* name)
	{
		ConVar* nameConvar = CVAR->FindVar(XorStr("name"));
		*reinterpret_cast<int*>(reinterpret_cast<DWORD>(&nameConvar->m_fnChangeCallbacks) + 0xC) = NULL;
		nameConvar->SetValue(name);
	}

#pragma region traces
	inline void TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, IClientEntity* ignore, trace_t* ptr)
	{
		Ray_t ray;
		ray.Init(vecAbsStart, vecAbsEnd);
		CTraceFilter filter;
		filter.pSkip = ignore;

		ENGINETRACE->TraceRay(ray, mask, &filter, ptr);
	}

	inline void TraceHull(Vector& src, Vector& end, Vector& mins, Vector& maxs, ITraceFilter* filter, trace_t* ptr)
	{
		Ray_t ray;
		ray.Init(src, end, mins, maxs);

		ENGINETRACE->TraceRay(ray, 0x200400B, filter, ptr);
	}

	inline bool IsVisible(Vector& StartPos, Vector& EndPos, C_CSPlayer* pLocal, C_CSPlayer* pEnt, bool autowall/* = false*/)
	{
		matrix3x4_t boneMatrix[MAXSTUDIOBONES];
		Vector eyePos = pLocal->GetEyePos();

		CGameTrace tr;
		Ray_t ray;
		CTraceFilter filter;
		filter.pSkip = pLocal;

		if (!pEnt->SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f))
			return false;

		auto studio_model = MODELINFO->GetStudioModel(pEnt->GetModel());

		if (!studio_model)
			return false;

		int scan_hitboxes[] = {
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
		};

		for (int i = 0; i < 20; i++)
		{
			auto hitbox = studio_model->GetHitboxSet(pEnt->GetPlayerHitboxSet())->pHitbox(scan_hitboxes[i]);
			if (hitbox)
			{
				auto
					min = Vector{},
					max = Vector{};

				Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

				ray.Init(eyePos, (min + max) * 0.5);
				ENGINETRACE->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

				if (tr.m_pEnt == pEnt || tr.fraction > 0.97f)
					return true;
			}
		}

		return false;

		/*trace_t tr;
		Ray_t ray;
		Vector vEndPos[3];
		Vector vStart = StartPos;
		Vector vEnd = EndPos;
		CTraceFilterSkipTwoEntities filter = CTraceFilterSkipTwoEntities(pLocal, pEnt);

		ray.Init(vStart, vEnd);
		ENGINETRACE->TraceRay(ray, MASK_SHOT, &filter, &tr);

		if (tr.fraction == 1.f || tr.m_pEnt == pEnt)
			return true;

		if (!autowall)
			return false;

		vEndPos[0] = tr.endpos;

		ray.Init(vEnd, vStart);
		ENGINETRACE->TraceRay(ray, MASK_SHOT, &filter, &tr);

		vEndPos[1] = tr.endpos;

		Math::VectorSubtract(vEndPos[0], vEndPos[1], vEndPos[2]);

		float flLength = vEndPos[2].Length();

		if (tr.fraction != 0)
			if (flLength < 14)
				return true;

		return false;*/
	}

	inline bool IsVisibleOld(Vector &Src, Vector &Dst, C_CSPlayer* skip) //skip = pLocal 99% of the time
	{
		Ray_t ray;
		ray.Init(Src, Dst);

		trace_t tr;
		CTraceFilter filter;
		filter.pSkip = skip;

		ENGINETRACE->TraceRay(ray, MASK_SHOT, &filter, &tr);

		return tr.m_pEnt != nullptr && tr.m_pEnt != ENTITYLIST->GetClientEntity(0);
	}

	inline bool LineGoesThroughSmoke(const Vector& start, const Vector& end, bool bUnknown = true)
	{
		typedef bool(*LineGoesThroughSmokeFn) (Vector, Vector, int16_t);
		static LineGoesThroughSmokeFn dw_fnLineGoesThroughSmoke;

		if (!dw_fnLineGoesThroughSmoke)
			dw_fnLineGoesThroughSmoke = (LineGoesThroughSmokeFn)FindPattern(get_client_dll(), XorStr("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"));

		return dw_fnLineGoesThroughSmoke(start, end, bUnknown);
	}

	inline void ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr)
	{

		static DWORD dwClipTraceToPlayer;

		while (!dwClipTraceToPlayer)
			dwClipTraceToPlayer = FindPattern(get_client_dll(), XorStr("53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 10"));

		_asm
		{
			MOV		EAX, filter
			LEA		ECX, tr
			PUSH	ECX
			PUSH	EAX
			PUSH	mask
			LEA		EDX, vecAbsEnd
			LEA		ECX, vecAbsStart
			CALL	dwClipTraceToPlayer
			ADD		ESP, 0xC
		}
	}
#pragma endregion

	inline float GetNetworkLatency()
	{
		INetChannelInfo *nci = ENGINE->GetNetChannelInfo();
		if (!nci)
			return 0.0f;

		return nci->GetLatency(FLOW_INCOMING);
		// we know the accurate latency for incoming since we just received it
		// would be different for outgoing because that has to be sent back from the server
	}

};
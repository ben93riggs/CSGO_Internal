#pragma once
#include "json.hpp"
#include "SourceEngine/SDK.hpp"
#include "XorStr.hpp"
#include "Globals.h"
#include <codecvt>
#include "Hooks.hpp"

using json = nlohmann::json;

inline std::wstring s2ws(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

inline std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

inline void VersionCheckCallback::Run(HTTPRequestCompleted_t* pvParam)
{
	if (pvParam->m_eStatusCode != 200)
	{
		//G::logger->info(XorStr("Authorization Failed! Status Code: {0:d}"), pvParam->m_eStatusCode);
		wchar_t buffer[256];
		wsprintfW(buffer, L"%d", pvParam->m_eStatusCode);
		MessageBox(nullptr, buffer, nullptr, 0);
		restore(AUTH_FAILURE);
		ExitThread(9);
	}

	std::string body;

	body.resize(pvParam->m_unBodySize + 1);

	auto body_cstr = reinterpret_cast<uint8_t*>(const_cast<char*>(body.data()));

	STEAMHTTP->GetHTTPResponseBodyData(pvParam->m_hRequest, body_cstr, pvParam->m_unBodySize);

	body_cstr[pvParam->m_unBodySize] = 0;
	
	//std::cout << body_cstr << std::endl;

	//printf("%d", pvParam->m_bRequestSuccessful);

	if (body.find(XorStr("SUCCESS")) == std::string::npos)
	{
		//G::logger->info(XorStr("Authorization Failed!"));
		//G::logger->info(XorStr(" Body: {0:s}\tSize: {1:d}\tRequest: {2:h}\tStatus Code: {3:d}\tSuccess: {4:d}"), body.c_str(), pvParam->m_unBodySize, pvParam->m_hRequest, pvParam->m_eStatusCode, pvParam->m_bRequestSuccessful);
		auto buffer = s2ws("Authorization Failed: " + body);
		MessageBox(nullptr, buffer.c_str(), nullptr, 0);
		restore(AUTH_FAILURE);
		ExitThread(9);
	}
	else
	{
		//G::logger->info(XorStr("Authorized!"));
	}

	STEAMHTTP->ReleaseHTTPRequest(pvParam->m_hRequest);
}

static VersionCheckCallback s_check_callback;

inline void SteamAPI_RegisterCallResult(VersionCheckCallback* callback, SteamAPICall_t handle)
{
	reinterpret_cast<void(__cdecl*)(VersionCheckCallback*, SteamAPICall_t)>(GetProcAddress(GetModuleHandleA(XorStr("steam_api.dll")), XorStr("SteamAPI_RegisterCallResult")))(callback, handle);
}

inline bool authorize()
{
	//HW_PROFILE_INFO hwProfileInfo;
	//DWORD HddNumber = 0;
	//GetCurrentHwProfile(&hwProfileInfo);// != NULL;
	//GetVolumeInformation(L"C://", nullptr, NULL, &HddNumber, nullptr, nullptr, nullptr, NULL);

	const auto handle = STEAMHTTP->CreateHTTPRequest(EHTTPMethod::POST, XorStr("//removed//"));
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "au", XorStr(""));
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "pcn", XorStr(""));
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "pcu", XorStr(""));
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "hdn", XorStr(""));
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "pfg", XorStr(""));
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "pfn", XorStr(""));
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "vr", CURRENTVERSION);

	SteamAPICall_t api_call;
	if (!STEAMHTTP->SendHTTPRequest(handle, &api_call))
	{
		restore(HTTP_FAILURE);
		return false;
	}

	SteamAPI_RegisterCallResult(&s_check_callback, api_call);

	return true;
}

inline bool PostError(RESTORE_MESSAGES error)
{
	std::string errormsg;
	switch (error)
	{
	case OK:
	case PROCESS_DETACH:
		return true;
	case AUTH_FAILURE:
		errormsg = XorStr("AUTH_FAILURE");
		break;
	case HTTP_FAILURE:
		errormsg = XorStr("HTTP_FAILURE");
		break;
	case DEBUG1:
		errormsg = XorStr("DEBUG1");
		break;
	case DEBUG2:
		errormsg = XorStr("DEBUG2");
		break;
	case DEBUG3:
		errormsg = XorStr("DEBUG3");
		break;
	case DEBUG4:
		errormsg = XorStr("DEBUG4");
		break;
	case DEBUG5:
		errormsg = XorStr("DEBUG5");
		break;
	case DEBUGGER_OLLY:
		errormsg = XorStr("DEBUGGER_OLLY");
		break;
	case DEBUGGER_WINDBG:
		errormsg = XorStr("DEBUGGER_WINDBG");
		break;
	case OBJECTLISTCHECK:
		errormsg = XorStr("OBJECTLISTCHECK");
		break;
	case CHECKHARDWAREBREAKPOINTS:
		errormsg = XorStr("CHECKHARDWAREBREAKPOINTS");
		break;
	case CHECKPROCESSDEBUGFLAGS:
		errormsg = XorStr("CHECKPROCESSDEBUGFLAGS");
		break;
	case DEBUGOBJECTCHECK:
		errormsg = XorStr("DEBUGOBJECTCHECK");
		break;
	case CANOPENCSRSS:
		errormsg = XorStr("CANOPENCSRSS");
		break;
	case CHECKGLOBALFLAGS:
		errormsg = XorStr("CHECKGLOBALFLAGS");
		break;
	case VALIDHDDSIZE:
		errormsg = XorStr("VALIDHDDSIZE");
		break;
	case SWALLOW_REDPILL:
		errormsg = XorStr("SWALLOW_REDPILL");
		break;
	case ISINSIDEVPC:
		errormsg = XorStr("ISINSIDEVPC");
		break;
	case ISINSIDEVMWARE:
		errormsg = XorStr("ISINSIDEVMWARE");
		break;
	case MEMORYBREAKPOINTDEBUGGERCHECK:
		errormsg = XorStr("MEMORYBREAKPOINTDEBUGGERCHECK");
		break;
	}

	HW_PROFILE_INFO hwProfileInfo;
	DWORD HddNumber = 0;
	GetCurrentHwProfile(&hwProfileInfo);// != NULL;
	GetVolumeInformation(L"C://", nullptr, NULL, &HddNumber, nullptr, nullptr, nullptr, NULL);

	auto handle = STEAMHTTP->CreateHTTPRequest(EHTTPMethod::POST, XorStr("//removed//"));
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "pcn", getenv(XorStr("COMPUTERNAME")));
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "pcu", getenv(XorStr("USERNAME")));
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "hdn", std::to_string(HddNumber).c_str());
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "pfg", ws2s(hwProfileInfo.szHwProfileGuid).c_str());
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "pfn", ws2s(hwProfileInfo.szHwProfileName).c_str());
	STEAMHTTP->SetHTTPRequestGetOrPostParameter(handle, "msg", errormsg.c_str());

	SteamAPICall_t api_call;

	return STEAMHTTP->SendHTTPRequest(handle, &api_call);
}
#pragma once
#include "SourceEngine/Convar.h"

class SpoofedConvar {
public:
	SpoofedConvar(const char* szCVar);
	SpoofedConvar(ConVar* pCVar);

	~SpoofedConvar();

	bool		IsSpoofed();
	void		Spoof();

	void		SetFlags(int flags);
	int			GetFlags();

	void		SetInt(int iValue);
	void		SetFloat(float flValue);
	void		SetString(const char* szValue);
	void		SetBool(bool bValue);

	int			GetInt();
	float		GetFloat();
	const char* GetString();

private:
	ConVar* m_pOriginalCVar = nullptr;
	ConVar* m_pDummyCVar = nullptr;

	char m_szDummyName[128];
	char m_szDummyValue[128];
	char m_szOriginalName[128];
	char m_szOriginalValue[128];
	int m_iOriginalFlags;
};
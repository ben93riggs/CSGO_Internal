#pragma once
#include "Singleton.h"

class C_CSPlayer;

class NameStealer : public singleton<NameStealer>
{
	bool wroteNameExploit = false;
	int callsAfterNameExploit = 0;

	void Start(C_CSPlayer * pLocal);
	void End();
public:
	void Do(C_CSPlayer * pLocal);
};


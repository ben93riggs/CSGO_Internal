#pragma once
#include "Singleton.h"

class C_CSPlayer;

class spammers : public singleton<spammers>
{
	static void location_spam(C_CSPlayer* p_local);
	static void report_spam(C_CSPlayer* p_local);

public:
	static void create_move();
};


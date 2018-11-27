#pragma once
#include "Singleton.h"

class buy_bot : public singleton<buy_bot>
{
public:
	static void run();
};
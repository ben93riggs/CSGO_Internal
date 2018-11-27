#include "BuyBot.h"
#include "SourceEngine/SDK.hpp"
#include "Vars.h"

void buy_bot::run()
{
	std::string buystring;

	if (MiscVars::get().g_nBuySecondary == BBS_DUALIES)
		buystring += XorStr("buy elite; ");
	if (MiscVars::get().g_nBuySecondary == BBS_DEAGLE_R8)
		buystring += XorStr("buy deagle; buy revolver; ");

	if (MiscVars::get().g_nBuyPrimary == BBP_AUTO)
		buystring += XorStr("buy g3sg1; buy scar20; ");
	else if (MiscVars::get().g_nBuyPrimary == BBP_SCOUT)
		buystring += XorStr("buy ssg08; ");
	else if (MiscVars::get().g_nBuyPrimary == BBP_AWP)
		buystring += XorStr("buy awp; ");

	if (buystring.length() > 3)
	{
		buystring += XorStr("buy taser 34; buy vesthelm; buy defuser; buy incgrenade; buy molotov; buy hegrenade; buy smokegrenade; buy flashbang;");
		ENGINE->ClientCmd(buystring.c_str());
	}
}

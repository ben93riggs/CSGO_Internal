#pragma once

class ClanTag : public singleton<ClanTag>
{
	std::vector<std::string> clantaglist =
	{
		XorStr("                 "),
		XorStr("                a"),
		XorStr("               aa"),
		XorStr("              aaa"),
		XorStr("             aaaa"),
		XorStr("            aaaaa"),
		XorStr("           aaaaaa"),
		XorStr("          aaaaaaa"),
		XorStr("         aaaaaaa."),
		XorStr("        aaaaaaa.b"),
		XorStr("       aaaaaaa.bb"),
		XorStr("      aaaaaaa.bbb"),
		XorStr("     aaaaaaa.bbbb"),
		XorStr("    aaaaaaa.bbbb "),
		XorStr("   aaaaaaa.bbbb  "),
		XorStr("  aaaaaaa.bbbb   "),
		XorStr(" aaaaaaa.bbbb    "),
		XorStr("aaaaaaa.bbbb     "),
		XorStr("aaaaaa.bbbb      "),
		XorStr("aaaaa.bbbb       "),
		XorStr("aaaa.bbbb        "),
		XorStr("aaa.bbbb         "),
		XorStr("aa.bbbb          "),
		XorStr("a.bbbb           "),
		XorStr(".bbbb            "),
		XorStr("bbbb             "),
		XorStr("bbb              "),
		XorStr("bb               "),
		XorStr("b                "),
		XorStr("                 "),
		XorStr("                 "),
		XorStr("                 ")
	};
	using sctFn = void(__fastcall*)(const char*, const char*);

	bool isSet = false;

public:
	void FrameStageNotify(ClientFrameStage_t curStage)
	{
		if (curStage != FRAME_NET_UPDATE_POSTDATAUPDATE_START)
			return;

		if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
			return;

		static sctFn pSetClanTag;
		
		if (!pSetClanTag)
			pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(Utils::FindSignature(XorStr("engine.dll"), XorStr("53 56 57 8B DA 8B F9 FF 15 ?? ?? ?? ??")));

		if (!pSetClanTag)
			return;

		if (!MiscVars::get().g_bClanTag && isSet)
		{
			pSetClanTag(XorStr(""), XorStr(""));
			isSet = false;
			return;
		}

		if (!MiscVars::get().g_bClanTag)
			return;

		static int timer = 0, real_time = 0;

		timer++;
		if (timer <= 32)
			return;
		
		timer = 0;

		pSetClanTag(clantaglist.at(real_time).c_str(), XorStr(""));
		isSet = true;

		real_time++;

		if (real_time > 31)
			real_time = 0;
	}
};
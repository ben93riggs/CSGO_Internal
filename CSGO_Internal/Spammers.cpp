#include "Spammers.h"
#include "Globals.h"

void spammers::location_spam(C_CSPlayer* p_local)
{
	// trying to make it so we can type in between memes
	static auto next_time = 0.f;
	const auto fl_server_time = p_local->GetTickBase() * GLOBALS->interval_per_tick;

	if (next_time > fl_server_time)
		return;

	next_time = fl_server_time + 1.0f + (MiscVars::get().g_bReportSpammer ? 0.5f : 0.0f);

	if (!MiscVars::get().g_bLocationSpammer)
		return;

	std::vector < C_CSPlayer* > players;
	for (int i = 0; i < 64; i++)
	{
		// Get the entity
		auto p_entity = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));

		if (!p_entity)
			continue;

		ClientClass* ent_client_class = p_entity->GetClientClass();

		if (!ent_client_class)
			continue;

		if (ent_client_class->m_ClassID != CCSPlayer)
			continue;

		// If it's a valid entity and isn't the player
		if (((MiscVars::get().g_iLocationSpammerMode == 2 && p_entity->GetTeam() == p_local->GetTeam()) || (MiscVars::get().g_iLocationSpammerMode != 2 && p_entity->GetTeam() != p_local->GetTeam())) && p_entity->IsAlive())
		{
			if (p_entity->sGetName() != XorStr("GOTV"))
				players.push_back(p_entity);
		}
	}

	if (!players.empty()) 
	{
		const int random_index = rand() % players.size();
		char sz_cmd[255];
		if (MiscVars::get().g_iLocationSpammerMode)
			sprintf(sz_cmd, "say %s is at %s with %d health!", players[random_index]->sGetName().c_str(), players[random_index]->GetLastPlace(), players[random_index]->GetHealth());
		else if (MiscVars::get().g_iLocationSpammerMode == 2)
			sprintf(sz_cmd, "say %s is at %s with %d health!", players[random_index]->sGetName().c_str(), players[random_index]->GetLastPlace(), players[random_index]->GetHealth());
		else
			sprintf(sz_cmd, "say_team %s is at %s with %d health!", players[random_index]->sGetName().c_str(), players[random_index]->GetLastPlace(), players[random_index]->GetHealth());

		ENGINE->ClientCmd(sz_cmd);
	}
}

void spammers::report_spam(C_CSPlayer * p_local)
{
	static auto next_time = 0.f;
	const auto fl_server_time = p_local->GetTickBase() * GLOBALS->interval_per_tick;

	if (next_time > fl_server_time)
		return;

	next_time = fl_server_time + 0.5f;

	if (!MiscVars::get().g_bReportSpammer)
		return;

	std::vector<C_CSPlayer*> players;

	for (int i = 0; i < 32; i++)
	{
		// Get the entity
		auto p_entity = dynamic_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));

		if (!p_entity)
			continue;

		const auto ent_client_class = p_entity->GetClientClass();

		if (!ent_client_class)
			continue;

		if (ent_client_class->m_ClassID != CCSPlayer)
			continue;

		// If it's a valid entity and isn't the player
		if (p_entity && p_entity->GetTeam() != p_local->GetTeam() && p_entity->IsAlive())
		{
			if (p_entity->sGetName() != XorStr("GOTV"))
				players.push_back(p_entity);
		}
	}

	if (!players.empty()) {
		const int random_index = rand() % players.size();
		char sz_cmd[255];
		static auto report_id = 3091961887844204733;
		sprintf(sz_cmd, "say Report for %s submitted, report id %lld.", players[random_index]->sGetName().c_str(), report_id);
		report_id += RandomInt(1, 3);

		ENGINE->ClientCmd(sz_cmd);
	}
}

void spammers::create_move()
{
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();
	if (!pLocal)
		return;

	location_spam(pLocal);
	report_spam(pLocal);
}

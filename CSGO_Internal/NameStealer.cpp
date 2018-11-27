#include "NameStealer.h"
#include "Config.h"

void NameStealer::Do(C_CSPlayer* pLocal)
{
	Start(pLocal);
	End();
}

void NameStealer::Start(C_CSPlayer* pLocal)
{
	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
	{
		wroteNameExploit = false;
		callsAfterNameExploit = 0;
		MiscVars::get().g_bNameStealer = false;
		return;
	}

	if (!MiscVars::get().g_bNameStealer || !wroteNameExploit || callsAfterNameExploit < 10)
		return;

	std::vector < C_CSPlayer* > players;
	for (int i = 0; i < 64; i++)
	{
		// Get the entity
		C_CSPlayer *pEntity = static_cast<C_CSPlayer*>(ENTITYLIST->GetClientEntity(i));

		if (!pEntity)
			continue;

		ClientClass* ent_client_class = pEntity->GetClientClass();

		if (!ent_client_class)
			continue;

		if (ent_client_class->m_ClassID != CCSPlayer)
			continue;

		// If it's a valid entity and isn't the player
		if (pEntity->GetTeam() != pLocal->GetTeam())
		{
			if (pEntity->sGetName().compare(XorStr("GOTV")) != 0)
				players.push_back(pEntity);
		}
	}

	if (players.size() > 0) {
		int randomIndex = rand() % players.size();

		char * newArray = new char[strlen(players[randomIndex]->sGetName().c_str()) + strlen("\n") + 1];
		strcpy(newArray, players[randomIndex]->sGetName().c_str());
		strcat(newArray, "\n");

		//find name cvar and set the new name
		ConVar* nameConvar = CVAR->FindVar(XorStr("name"));
		*reinterpret_cast<int*>(reinterpret_cast<DWORD>(&nameConvar->m_fnChangeCallbacks) + 0xC) = NULL;
		nameConvar->SetValue(newArray);

		//delete the char array when finished
		delete[] newArray;
	}
}

void NameStealer::End()
{
	if (MiscVars::get().g_bNameStealer && !wroteNameExploit)
	{
		// write name exploit. Might get patched soon.
		ConVar* nameConvar = CVAR->FindVar(XorStr("name"));
		*reinterpret_cast<int*>(reinterpret_cast<DWORD>(&nameConvar->m_fnChangeCallbacks) + 0xC) = NULL;
		nameConvar->SetValue(XorStr("\nญญญ"));

		// Only needs to be written once.
		wroteNameExploit = true;
	}
	else if (wroteNameExploit && callsAfterNameExploit < 10)
	{
		callsAfterNameExploit++;
	}
}

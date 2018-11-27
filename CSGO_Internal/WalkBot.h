#pragma once
class WalkBot
{
public:
	void cCLocalPlayer::AutoWalk(CUserCmd* cmd)
	{
		//////////////////////////////////////////////////////
		//Walk Luke Walk
		//////////////////////////////////////////////////////
		static bool bWalking = false;
		if (Main.m_pCheatCvars->aim_walkbot.GetBool())
		{
			//////////////////////////////////////////////////////
			//Choose Luke Choose
			//////////////////////////////////////////////////////    
			if (!m_vecWaypoints.empty() && m_vecWaypoints.size())
			{
				if (cmd->buttons & IN_ATTACK)
				{
					Main.m_pinterpreter->Execute("#-forward");
					iTheActualWaypoint = 0;
					m_bWpAiming = false;
					bWalking = false;
					return;
				}
				if (iTheActualWaypoint == 0)
				{
					for (unsigned int i = 1; i < m_vecWaypoints.size(); ++i)
					{
						if (iTheActualWaypoint == (int)i)continue;

						if (Main.m_pEntity->GetDistance(m_vecWaypoints[i]) < Main.m_pEntity->GetDistance(m_vecWaypoints[iTheActualWaypoint]))
							iTheActualWaypoint = i;
					}
				}
				if (Main.m_pEntity->GetVisible(m_vecWaypoints[iTheActualWaypoint]))
				{
					if (Main.m_pEntity->GetDistance(m_vecWaypoints[iTheActualWaypoint]) < 10.0f)
					{
						bool bIsInBuyZone = (bool)Main.m_pMattAutoit->GetClass_int(Main.m_pMyPlayer->BaseEnt(), Main.m_pMattAutoit->m_bInBuyZone_Offs);
						if (!bIsInBuyZone)//Play Sound clik clik ;)
							Main.m_pinterpreter->Execute("#autobuy");

						++iTheActualWaypoint;

						if (iTheActualWaypoint == (int)m_vecWaypoints.size())
							iTheActualWaypoint = 0;

						QAngle qView;
						Main.m_pEntity->CalcAngle(Main.m_pEntity->GetEyePosition(Main.m_pMyPlayer->BaseEnt()), m_vecWaypoints[iTheActualWaypoint], qView);
						Main.m_phk_Engine->SetViewAngles(qView);
						m_bWpAiming = true;
						//////////////////////////////////////////////////////
						//Walk Luke Walk
						//////////////////////////////////////////////////////    
						if (m_bWpAiming)
						{
							Main.m_pinterpreter->Execute("#+forward");
							bWalking = true;
						}
					}
				}
			}
			//////////////////////////////////////////////////////
			//Stop Luke Stop
			//////////////////////////////////////////////////////    
			if (bWalking && !m_bWpAiming)
			{
				Main.m_pinterpreter->Execute("#-forward");
				iTheActualWaypoint = 0;
				m_bWpAiming = false;
				bWalking = false;
			}
		}
		else
		{
			//////////////////////////////////////////////////////
			//Stop Luke Stop
			//////////////////////////////////////////////////////    
			if (bWalking)
			{
				Main.m_pinterpreter->Execute("#-forward");
				iTheActualWaypoint = 0;
				m_bWpAiming = false;
				bWalking = false;
			}
		}
	}



public:
	bool    m_bWpAiming;
	unsigned int iTheActualWaypoint;
	std::vector<Vector>m_vecWaypoints;












	void func_wpadd(void)
	{
		if (!Main.m_phk_Engine->IsInGame())return;
		Main.m_pMyPlayer->m_vecWaypoints.push_back(Main.m_pEntity->GetEyePosition(Main.m_pMyPlayer->BaseEnt()));
		Main.m_pCheatConsole->Printf("Waypoint added.");
	}

	void func_wpload(void)
	{
		if (!Main.m_phk_Engine->IsInGame())return;
		Vector addme;
		addme.x = Main.m_pinterpreter->flGetWord(1);
		addme.y = Main.m_pinterpreter->flGetWord(2);
		addme.z = Main.m_pinterpreter->flGetWord(3);
		Main.m_pMyPlayer->m_vecWaypoints.push_back(addme);
		Main.m_pCheatConsole->Printf("&bWaypoint &d%f &e%f &f%f added.", addme.x, addme.y, addme.z);
	}

	void func_wpclear(void)
	{
		if (!Main.m_phk_Engine->IsInGame())return;
		Main.m_pMyPlayer->m_vecWaypoints.clear();
	}

	void func_wpsave(void)
	{
		if (!Main.m_phk_Engine->IsInGame())return;

		char szBuff[256] = { '' };
		std::ofstream saveofs(Main.In_Hack_Folder(Main.m_pinterpreter->pszGetWord(1)).c_str());
		for (int i = 0; i < Main.m_pMyPlayer->m_vecWaypoints.size(); ++i)
		{
			sprintf(szBuff, "wpload %f %f %fn", Main.m_pMyPlayer->m_vecWaypoints[i].x, Main.m_pMyPlayer->m_vecWaypoints[i].y, Main.m_pMyPlayer->m_vecWaypoints[i].z);
			saveofs << szBuff << ' ' << 'n';
		}
	}
};


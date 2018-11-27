#pragma once
#include "checksum_md5.h"

class CUserCmd;

class PredictionSystem : public singleton<PredictionSystem>
{
	float m_flOldCurtime;
	float m_flOldFrametime;
	int m_nOldTickBase;
	int* m_nPredictionRandomSeed = nullptr;

public:
	void Start(CUserCmd* cmd)
	{
		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
			return;

		//get pointer to seed
		if (!m_nPredictionRandomSeed)
			m_nPredictionRandomSeed = *(int**)(Utils::FindSignature(Utils::get_client_dll(), XorStr("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")) + 2);

		//backup
		m_nOldTickBase = pLocal->GetTickBase();
		m_flOldCurtime = GLOBALS->curtime;
		m_flOldFrametime = GLOBALS->frametime;
		//set seed
		*m_nPredictionRandomSeed = MD5_PseudoRandom(cmd->command_number) & INT_MAX;

		//set times
		GLOBALS->curtime = pLocal->GetTickBase() * GLOBALS->interval_per_tick;
		GLOBALS->frametime = GLOBALS->interval_per_tick;

		//??
		GAMEMOVEMENT->StartTrackPredictionErrors(pLocal);

		//zero out move data
		CMoveData m_MoveData;
		memset(&m_MoveData, 0, sizeof(m_MoveData));

		//do prediction
		Interface->MoveHelper()->SetHost(pLocal);
		PREDICTION->SetupMove(pLocal, cmd, Interface->MoveHelper(), &m_MoveData);
		GAMEMOVEMENT->ProcessMovement(pLocal, &m_MoveData);
		pLocal->GetTickBase() = m_nOldTickBase;
		PREDICTION->FinishMove(pLocal, cmd, &m_MoveData);
	}

	void Finish() const
	{
		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)
			return;

		GAMEMOVEMENT->FinishTrackPredictionErrors(pLocal);
		Interface->MoveHelper()->SetHost(nullptr);

		*m_nPredictionRandomSeed = -1;

		GLOBALS->curtime = m_flOldCurtime;
		GLOBALS->frametime = m_flOldFrametime;
	}

};


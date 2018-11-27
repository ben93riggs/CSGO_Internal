#include "LagCompensation.h"
#include "SourceEngine/SDK.hpp"

//call in createmove 
void LagCompensation::UpdateIncomingSequences()
{
	if (!CLIENTSTATE->net_channel)
		return;


	if (m_nInSequenceNumber_backup == 0)
	{
		m_nInSequenceNumber_backup = CLIENTSTATE->net_channel->m_nInSequenceNr;
		lastincomingsequencenumber = CLIENTSTATE->net_channel->m_nInSequenceNr;
		//LocalPlayer.ModifiedSequenceNr = false;
	}

	if (CLIENTSTATE->net_channel->m_nInSequenceNr > lastincomingsequencenumber)
	{
		//sequences.push_front(netchan->m_nInSequenceNr);
		lastincomingsequencenumber = CLIENTSTATE->net_channel->m_nInSequenceNr;

		sequences.push_front(CIncomingSequence(CLIENTSTATE->net_channel->m_nInReliableState, CLIENTSTATE->net_channel->m_nOutReliableState, CLIENTSTATE->net_channel->m_nInSequenceNr, GLOBALS->realtime));
	}

	if (sequences.size() > 2048)
		sequences.pop_back();


}

void LagCompensation::ClearIncomingSequences()
{
	m_nInSequenceNumber_backup = 0.0f;
	sequences.clear();
}

//Adds latency to the local player
void LagCompensation::AddLatencyToNetchan(INetChannel *netchan, float Latency)
{
	for (auto& seq : sequences)
	{
		if (GLOBALS->realtime - seq.curtime >= Latency)
		{
			netchan->m_nInReliableState = seq.inreliablestate;
			netchan->m_nInSequenceNr = seq.sequencenr;
			break;
		}
	}
}
#pragma once
#include "Singleton.h"
#include <deque>

class INetChannel;

struct CIncomingSequence
{
	CIncomingSequence::CIncomingSequence(int instate, int outstate, int seqnr, float time)
	{
		inreliablestate = instate;
		outreliablestate = outstate;
		sequencenr = seqnr;
		curtime = time;
	}
	int inreliablestate;
	int outreliablestate;
	int sequencenr;
	float curtime;
};

class LagCompensation : public singleton<LagCompensation>
{
public:
	std::deque<CIncomingSequence>sequences;
	int lastincomingsequencenumber;
	int m_nInSequenceNumber_backup = 0;

	//call in createmove 
	void UpdateIncomingSequences();

	void ClearIncomingSequences();

	//Adds latency to the local player
	void AddLatencyToNetchan(INetChannel *netchan, float Latency);
};
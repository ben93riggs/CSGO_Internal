#pragma once
#include <cstdint>

//class INetMessage
//{
//public:
//	virtual void	INetMessage_Destructor() {};
//
//	// Use these to setup who can hear whose voice.
//	// Pass in client indices (which are their ent indices - 1).
//
//	virtual void	SetNetChannel(INetChannel * netchan) = 0; // netchannel this message is from/for
//	virtual void	SetReliable(bool state) = 0;	// set to true if it's a reliable message
//
//	virtual bool	Process(void) = 0; // calles the recently set handler to process this message
//
//	virtual	bool	ReadFromBuffer(bf_read &buffer) = 0; // returns true if parsing was OK
//	virtual	bool	WriteToBuffer(bf_write &buffer) = 0;	// returns true if writing was OK
//
//	virtual bool	IsReliable(void) const = 0;  // true, if message needs reliable handling
//
//	virtual int				GetType(void) const = 0; // returns module specific header tag eg svc_serverinfo
//	virtual int				GetGroup(void) const = 0;	// returns net message group of this message
//	virtual const char		*GetName(void) const = 0;	// returns network message name, eg "svc_serverinfo"
//	virtual INetChannel		*GetNetChannel(void) const = 0;
//	virtual const char		*ToString(void) const = 0; // returns a human readable string about message content
//};

class INetChannel
{
public:
	char pad_0x0000[0x18]; //0x0000
	int m_nOutSequenceNr; //0x0018 
	int m_nInSequenceNr; //0x001C 
	int m_nOutSequenceNrAck; //0x0020 
	int m_nOutReliableState; //0x0024 
	int m_nInReliableState; //0x0028 
	int m_nChokedPackets; //0x002C 
	char pad_0x0030[0x6C]; //0x0030
	int m_Socket; //0x009C 
	int m_StreamSocket; //0x00A0 
	int m_MaxReliablePayloadSize; //0x00A4 
	char pad_0x00A8[0x64]; //0x00A8
	float last_received; //0x010C 
	float connect_time; //0x0110 
	char pad_0x0114[0x4]; //0x0114
	int m_Rate; //0x0118 
	char pad_0x011C[0x4]; //0x011C
	float m_fClearTime; //0x0120 
	char pad_0x0124[0x4150]; //0x0124
	int m_ChallengeNr; //0x4274 
	float m_flTimeout; //0x4278 
	char pad_0x427C[0x24]; //0x427C
	float m_flUnknown; //0x42A0 
	float m_flInterpolationAmount; //0x42A4 
	float m_flRemoteFrameTime; //0x42A8 
	float m_flRemoteFrameTimeStdDeviation; //0x42AC 
	int m_nMaxRoutablePayloadSize; //0x42B0 
	int m_nSplitPacketSequence; //0x42B4 
	char pad_0x42B8[0x28]; //0x42B8
	bool m_bIsValveDS; //0x42E0 

	bool SendNetMsg(/*INetMessage*/void* msg, bool bForceReliable, bool bVoice) 
	{
		typedef bool(__thiscall* func1)(void*, /*INetMessage*/void*, bool, bool);
		return CallVFunction<func1>(this, 42)(this, msg, bForceReliable, bVoice);
	}
}; //Size=0x42E1

class CClockDriftMgr
{
public:
	float m_ClockOffsets[16];   //0x0000
	uint32_t m_iCurClockOffset; //0x0040
	uint32_t m_nServerTick;     //0x0044
	uint32_t m_nClientTick;     //0x0048
}; //Size: 0x4C

class CEventInfo
{
public:

	uint16_t classID; //0x0000 0 implies not in use
	char pad_0002[2]; //0x0002 
	float fire_delay; //0x0004 If non-zero, the delay time when the event should be fired ( fixed up on the client )
	char pad_0008[4]; //0x0008
	ClientClass *pClientClass; //0x000C
	void *pData; //0x0010 Raw event data
	char pad_0014[48]; //0x0014
}; //Size: 0x0044

class IClientState {
public:											//all offsets need +0x4
	char			pad[0x9C];					//0x0
	INetChannel*	net_channel;				//0x9C
	uint32_t		m_nChallengeNr;				//0xA0
	char			pad_00A4[0x64];			    //0xA4
	int				m_nSignonState;				//0x108
	char			pad3[0x8];					//0x10c
	float			m_flNextCmdTime;			//0x114
	int				m_nServerCount;				//0x118
	int				m_nCurrentSequence;			//0x11C
	char			pad_4[0x8];					//0x0120
	CClockDriftMgr	m_ClockDriftMgr;			//0x0128
	int				m_nDeltaTick;				//0x0174
	bool			m_bPaused;					//0x0178
	char			pad_017D[0x3];				//0x0179
	int				view_entity;				//0x017C
	uint32_t		m_nPlayerSlot;				//0x0180
	char			m_szLevelName[0x104];       //0x0184
	char			m_szLevelNameShort[0x28];   //0x0288
	char			m_szGroupName[0x28];        //0x02B0
	char			pad_02DC[0x38];             //0x02D8
	uint32_t		m_nMaxClients;				//0x0310
	char			pad_0314[0x4984];		    //0x0314
	float			m_flLastServerTickTime;     //0x4C98
	bool			insimulation;               //0x4C9C
	char			pad_4C9D[0x3];              //0x4C9D
	uint32_t		oldtickcount;               //0x4CA0
	float			m_tickRemainder;            //0x4CA4
	float			m_frameTime;                //0x4CA8
	uint32_t		lastoutgoingcommand;		//0x4CAC
	uint32_t		chokedcommands;				//0x4CB0
	uint32_t		last_command_ack;			//0x4CB4
	uint32_t		command_ack;				//0x4CB8
	uint32_t		m_nSoundSequence;			//0x4CBC
	char			pad_4CC0[0x50];             //0x4CC0
	Vector			viewangles;					//0x4D10
	char			pad_4D1C[208]; //0x4D1C
	CEventInfo*		events; //0x4DEC
	//char			pad_4D1C[196];              //0x4D1C
	//uintptr_t		events;						//0x4DE0

	void force_full_update() 
	{
		if (m_nDeltaTick != -1)
			m_nDeltaTick = -1;
	}
};

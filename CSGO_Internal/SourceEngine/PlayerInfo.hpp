#pragma once

	typedef struct player_info_s {
		DWORD __pad0[2];
		int m_nXuidLow;
		int m_nXuidHigh;
		char m_szPlayerName[128];
		int m_nUserID;
		char m_szSteamID[33];
		UINT m_nSteam3ID;
		char m_szFriendsName[128];
		bool m_bIsFakePlayer;
		bool m_bIsHLTV;
		DWORD m_dwCustomFiles[4];
		BYTE m_FilesDownloaded;
		int __pad1;
	} player_info_t;
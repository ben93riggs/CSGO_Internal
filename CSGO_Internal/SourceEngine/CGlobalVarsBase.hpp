#pragma once

     class CGlobalVarsBase {
     public:
          float     realtime;								//0x0
          int       framecount;								//0x4
          float     absoluteframetime;						//0x8
          float     absoluteframestarttimestddev;			//0xC
          float     curtime;								//0x10
          float     frametime;								//0x14
          int       maxClients;								//0x18
          int       tickcount;								//0x1C
          float     interval_per_tick;						//0x20
          float     interpolation_amount;					//0x24
          int       simTicksThisFrame;						//0x28
          int       network_protocol;						//0x2C
          void*     pSaveData;								//0x30
          bool      m_bClient;								//0x34
          bool      m_bRemoteClient;						//0x35

     private:
          // 100 (i.e., tickcount is rounded down to this base and then the "delta" from this base is networked
          int       nTimestampNetworkingBase;				//0x36
          // 32 (entindex() % nTimestampRandomizeWindow ) is subtracted from gpGlobals->tickcount to set the networking basis, prevents
          //  all of the entities from forcing a new PackedEntity on the same tick (i.e., prevents them from getting lockstepped on this)
          int       nTimestampRandomizeWindow;				//0x3A
     }; //size=0x3E
#pragma once

     class IClientNetworkable;
     class IClientEntity;

     class IClientEntityList {
     public:
          virtual IClientNetworkable*   GetClientNetworkable( int entnum ) = 0;
          virtual void*                 GetClientNetworkableFromHandle( void ) = 0;
          virtual void*                 GetClientUnknownFromHandle( void ) = 0;
          virtual IClientEntity*        GetClientEntity( int entNum ) = 0;
          virtual IClientEntity*        GetClientEntityFromHandle( CBaseHandle hEnt ) = 0;
          virtual int                   NumberOfEntities( bool bIncludeNonNetworkable ) = 0;
          virtual int                   GetHighestEntityIndex( void ) = 0;
          virtual void                  SetMaxEntities( int maxEnts ) = 0;
          virtual int                   GetMaxEntities() = 0;
     };
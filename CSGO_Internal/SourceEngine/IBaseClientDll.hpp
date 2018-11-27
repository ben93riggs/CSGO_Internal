#pragma once

#include "Definitions.hpp"

#include "CGlobalVarsBase.hpp"
#include "ClientClass.hpp"

class IBaseClientDLL //ICHLCLient
{
public:
	virtual int              Connect(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0; // Connect appsystem components, get global interfaces, don't run any other init code
	virtual int              Disconnect() = 0;
	virtual int              Init(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
	virtual void             PostInit() = 0;
	virtual void             Shutdown() = 0;
	virtual void             LevelInitPreEntity(char const* pMapName) = 0;
	virtual void             LevelInitPostEntity() = 0;
	virtual void             LevelShutdown() = 0;
	virtual ClientClass*     GetAllClasses() = 0;
};


#pragma once

#include "IAppSystem.hpp"
#include "IConVar.hpp"

	class ConCommandBase;
	class ConCommand;
	class ConVar;

	typedef int CVarDLLIdentifier_t;

	class IConsoleDisplayFunc
	{
	public:
		virtual void ColorPrint(const uint8_t* clr, const char *pMessage) = 0;
		virtual void Print(const char *pMessage) = 0;
		virtual void DPrint(const char *pMessage) = 0;
	};

	class ICvar : public IAppSystem
	{
	public:
		virtual CVarDLLIdentifier_t        AllocateDLLIdentifier() = 0; // 0x0
		virtual void                       RegisterConCommand(ConCommandBase *pCommandBase) = 0; // 0x4
		virtual void                       UnregisterConCommand(ConCommandBase *pCommandBase) = 0; // 0x8
		virtual void                       UnregisterConCommands(CVarDLLIdentifier_t id) = 0; // 0xC
		virtual const char*                GetCommandLineValue(const char *pVariableName) = 0; // 0x10
		virtual ConCommandBase*            FindCommandBase(const char *name) = 0; // 0x14
		virtual const ConCommandBase*      FindCommandBase(const char *name) const = 0; // 0x18
		virtual ConVar*                    FindVar(const char *var_name) = 0; // 0x1C
		virtual const ConVar*              FindVar(const char *var_name) const = 0; // 0x20
		virtual ConCommand*                FindCommand(const char *name) = 0; // 0x24
		virtual const ConCommand*          FindCommand(const char *name) const = 0; // 0x28
		virtual void                       InstallGlobalChangeCallback(FnChangeCallback_t callback) = 0; // 0x2C
		virtual void                       RemoveGlobalChangeCallback(FnChangeCallback_t callback) = 0; // 0x30
		virtual void                       CallGlobalChangeCallbacks(ConVar *var, const char *pOldString, float flOldValue) = 0; // 0x34
		virtual void                       InstallConsoleDisplayFunc(IConsoleDisplayFunc* pDisplayFunc) = 0; // 0x38
		virtual void                       RemoveConsoleDisplayFunc(IConsoleDisplayFunc* pDisplayFunc) = 0; // 0x3C
		virtual void                       ConsoleColorPrintf(const uint8_t* clr, const char *pFormat, ...) const = 0; // 0x40
		virtual void                       ConsolePrintf(const char *pFormat, ...) const = 0; // 0x44
		virtual void                       ConsoleDPrintf(const char *pFormat, ...) const = 0; // 0x48
		virtual void                       RevertFlaggedConVars(int nFlag) = 0; // 0x4C
	};

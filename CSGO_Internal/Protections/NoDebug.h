#pragma once
#include <bcrypt.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include "VirtualizerSDK.h"
#include "../Hooks.hpp"

#ifndef RELEASE_VERSION
#define DBMB(_x_)//MessageBoxA(NULL, _x_, "dbgcheck error", MB_OK)
#else
#define DBMB(_x_)
#endif

#define BADPROCESS(procname) if (GetProcessIdFromName(procname)) { DBMB(procname); restore(OBJECTLISTCHECK); exit(1); }

using namespace hooks;

// Grabbed this definition of MSDN and modified one pointer
// http://msdn.microsoft.com/en-us/library/ms684280(VS.85).aspx
typedef struct _PROCESS_BASIC_INFORMATION {
	PVOID Reserved1;
	void* PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	ULONG_PTR ParentProcessId;
} PROCESS_BASIC_INFORMATION;

// Taken from: http://msdn.microsoft.com/en-us/library/
// aa380518(VS.85).aspx
typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING,
UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING TypeName;
	ULONG TotalNumberOfHandles;
	ULONG TotalNumberOfObjects;
}OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

// Returned by the ObjectAllTypeInformation class
// passed to NtQueryObject
typedef struct _OBJECT_ALL_INFORMATION {
	ULONG NumberOfObjects;
	OBJECT_TYPE_INFORMATION ObjectTypeInformation[1];
}OBJECT_ALL_INFORMATION, *POBJECT_ALL_INFORMATION;

inline bool Int2DCheck()
{
	__try
	{
		__asm
		{
			int 0x2d
			xor eax, eax
			add eax, 2
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

inline bool CheckForCCBreakpoint(void* pMemory, size_t SizeToCheck)
{
	unsigned char *pTmp = (unsigned char*)pMemory;
	for (size_t i = 0; i < SizeToCheck; i++)
	{
		if (pTmp[i] == 0xCC)
			return true;
	}

	return false;
} //

inline bool CheckForCCBreakpointXor55(void* pMemory, size_t SizeToCheck)
{
	unsigned char *pTmp = (unsigned char*)pMemory;
	unsigned char tmpchar = 0;

	for (size_t i = 0; i < SizeToCheck; i++)
	{
		tmpchar = pTmp[i];
		if (0x99 == (tmpchar ^ 0x55)) // 0xCC xor 0x55 = 0x99
			return true;
	}

	return false;
}

inline bool MemoryBreakpointDebuggerCheck()
{
	unsigned char *pMem = NULL;
	SYSTEM_INFO sysinfo = { 0 };
	DWORD OldProtect = 0;
	void *pAllocation = NULL; // Get the page size for the system 

	GetSystemInfo(&sysinfo); // Allocate memory 

	pAllocation = VirtualAlloc(NULL, sysinfo.dwPageSize,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE);

	if (pAllocation == NULL)
		return false;

	// Write a ret to the buffer (opcode 0xc3)
	pMem = (unsigned char*)pAllocation;
	*pMem = 0xc3;

	// Make the page a guard page         
	if (VirtualProtect(pAllocation, sysinfo.dwPageSize,
		PAGE_EXECUTE_READWRITE | PAGE_GUARD,
		&OldProtect) == 0)
	{
		return false;
	}

	__try
	{
		__asm
		{
			mov eax, pAllocation
			// This is the address we'll return to if we're under a debugger
			push MemBpBeingDebugged
			jmp eax // Exception or execution, which shall it be :D?
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		// The exception occured and no debugger was detected
		VirtualFree(pAllocation, NULL, MEM_RELEASE);
		return false;
	}

	__asm {MemBpBeingDebugged:}
	VirtualFree(pAllocation, NULL, MEM_RELEASE);
	return true;
}

// CheckHardwareBreakpoints returns the number of hardware 
// breakpoints detected and on failure it returns -1.
inline int CheckHardwareBreakpoints()
{
	unsigned int NumBps = 0;

	// This structure is key to the function and is the 
	// medium for detection and removal
	CONTEXT ctx;
	ZeroMemory(&ctx, sizeof(CONTEXT));

	// The CONTEXT structure is an in/out parameter therefore we have
	// to set the flags so Get/SetThreadContext knows what to set or get.
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

	// Get a handle to our thread
	HANDLE hThread = GetCurrentThread();

	// Get the registers
	if (GetThreadContext(hThread, &ctx) == 0)
		return -1;

	// Now we can check for hardware breakpoints, its not 
	// necessary to check Dr6 and Dr7, however feel free to
	if (ctx.Dr0 != 0)
		++NumBps;
	if (ctx.Dr1 != 0)
		++NumBps;
	if (ctx.Dr2 != 0)
		++NumBps;
	if (ctx.Dr3 != 0)
		++NumBps;

	return NumBps;
}

#define SERIAL_THRESHOLD 0x10000 // 10,000h ticks

inline DWORD GenerateSerial(TCHAR* pName)
{
	DWORD LocalSerial = 0;

	DWORD RdtscLow = 0; // TSC Low
	__asm
	{
		rdtsc
		mov RdtscLow, eax
	}

	size_t strlen = wcslen(pName);

	// Generate serial 

	for (unsigned int i = 0; i < strlen; i++)
	{
		LocalSerial += (DWORD)pName[i];
		LocalSerial ^= 0xDEADBEEF;
	}

	__asm
	{
		rdtsc
		sub eax, RdtscLow
		cmp eax, SERIAL_THRESHOLD
		jbe NotDebugged
		push 0
		call ExitProcess
		NotDebugged :
	}
	return LocalSerial;
}

#define SERIAL_THRESHOLD 0x10000 // 10,000h ticks

inline DWORD GenerateSerialWin32Attack(TCHAR* pName)
{
	DWORD LocalSerial = 0;
	size_t strlen = wcslen(pName);

	DWORD Counter = GetTickCount(); // Could be replaced with timeGetTime()

									// Generate serial 
	for (unsigned int i = 0; i < strlen; i++)
	{
		LocalSerial += (DWORD)pName[i];
		LocalSerial ^= 0xDEADBEEF;
	}

	Counter = GetTickCount() - Counter; // Could be replaced with timeGetTime() 
	if (Counter >= SERIAL_THRESHOLD)
		ExitProcess(0);

	return LocalSerial;
}

inline void PushPopSS()
{
	__asm
	{
		push ss
		pop ss
		mov eax, 9 // This line executes but is stepped over
		xor edx, edx // This is where the debugger will step to
	}
}

// CheckProcessDebugFlags will return true if 
// the EPROCESS->NoDebugInherit is == FALSE, 
// the reason we check for false is because 
// the NtQueryProcessInformation function returns the
// inverse of EPROCESS->NoDebugInherit so (!TRUE == FALSE)
inline bool CheckProcessDebugFlags()
{
	// Much easier in ASM but C/C++ looks so much better
	typedef NTSTATUS(WINAPI *pNtQueryInformationProcess)
		(HANDLE, UINT, PVOID, ULONG, PULONG);

	DWORD NoDebugInherit = 0;
	NTSTATUS Status;

	// Get NtQueryInformationProcess
	pNtQueryInformationProcess NtQIP = (pNtQueryInformationProcess)
		GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")),
			"NtQueryInformationProcess");

	Status = NtQIP(GetCurrentProcess(),
		0x1f, // ProcessDebugFlags
		&NoDebugInherit, 4, NULL);

	if (Status != 0x00000000)
		return false;

	if (NoDebugInherit == FALSE)
		return true;
	else
		return false;
}

inline bool DebugObjectCheck()
{
	// Much easier in ASM but C/C++ looks so much better
	typedef NTSTATUS(WINAPI *pNtQueryInformationProcess)
		(HANDLE, UINT, PVOID, ULONG, PULONG);

	HANDLE hDebugObject = NULL;
	NTSTATUS Status;

	// Get NtQueryInformationProcess
	pNtQueryInformationProcess NtQIP = (pNtQueryInformationProcess)
		GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")),
			"NtQueryInformationProcess");

	Status = NtQIP(GetCurrentProcess(),
		0x1e, // ProcessDebugObjectHandle
		&hDebugObject, 4, NULL);

	if (Status != 0x00000000)
		return false;

	if (hDebugObject)
		return true;
	else
		return false;
}

inline bool HideThread(HANDLE hThread)
{
	typedef NTSTATUS(NTAPI *pNtSetInformationThread)
		(HANDLE, UINT, PVOID, ULONG);
	NTSTATUS Status;

	// Get NtSetInformationThread
	pNtSetInformationThread NtSIT = (pNtSetInformationThread)
		GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")),
			"NtSetInformationThread");

	// Shouldn't fail
	if (NtSIT == NULL)
		return false;

	// Set the thread info
	if (hThread == NULL)
		Status = NtSIT(GetCurrentThread(),
			0x11, // HideThreadFromDebugger
			0, 0);
	else
		Status = NtSIT(hThread, 0x11, 0, 0);

	if (Status != 0x00000000)
		return false;
	else
		return true;
}

inline bool CheckOutputDebugString(LPCTSTR String)
{
	OutputDebugString(String);
	if (GetLastError() == 0)
		return true;
	else
		return false;
}

#pragma region VM Detection
inline bool ValidHDDSize()
{
	BOOL ret = FALSE;
	ULARGE_INTEGER ulFreeSpace;
	ULARGE_INTEGER ulTotalSpace;
	ULARGE_INTEGER ulTotalFreeSpace;
	__int64 ulTotalUsedSpace = 0;
	GetDiskFreeSpaceEx(NULL, &ulFreeSpace, &ulTotalSpace, &ulTotalFreeSpace);
	ULONGLONG diskSize = ulTotalSpace.QuadPart;

	if (((diskSize / 1024) / 1024) / 1024 < 100)
		return false;
	return true;
}

inline bool swallow_redpill() {
	unsigned char m[2 + 4], rpill[] = "\x0f\x01\x0d\x00\x00\x00\x00\xc3";
	*(unsigned*)&rpill[3] = (unsigned)m;
	((void(*)())&rpill)();
	return m[5]>0xd0;
}

// IsInsideVPC's exception filter
DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep)
{
	PCONTEXT ctx = ep->ContextRecord;

	ctx->Ebx = -1; // Not running VPC
	ctx->Eip += 4; // skip past the "call VPC" opcodes
	return EXCEPTION_CONTINUE_EXECUTION;
	// we can safely resume execution since we skipped faulty instruction
}

// High level language friendly version of IsInsideVPC()
inline bool IsInsideVPC()
{
	bool rc = false;

	__try
	{
		_asm push ebx
		_asm mov  ebx, 0 // It will stay ZERO if VPC is running
		_asm mov  eax, 1 // VPC function number

						 // call VPC 
		_asm __emit 0Fh
		_asm __emit 3Fh
		_asm __emit 07h
		_asm __emit 0Bh

		_asm test ebx, ebx
		_asm setz[rc]
			_asm pop ebx
	}
	// The except block shouldn't get triggered if VPC is running!!
	__except (IsInsideVPC_exceptionFilter(GetExceptionInformation()))
	{
	}

	return rc;
}

inline bool IsInsideVMWare()
{
	bool rc = true;

	__try
	{
		__asm
		{
			push   edx
			push   ecx
			push   ebx

			mov    eax, 'VMXh'
			mov    ebx, 0 // any value but not the MAGIC VALUE
			mov    ecx, 10 // get VMWare version
			mov    edx, 'VX' // port number

			in     eax, dx // read port
						   // on return EAX returns the VERSION
						   cmp    ebx, 'VMXh' // is it a reply from VMWare?
						   setz[rc] // set return value

						   pop    ebx
						   pop    ecx
						   pop    edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		rc = false;
	}

	return rc;
}

#pragma endregion

// This function uses the toolhelp32 api to enumerate all running processes
// on the computer and does a comparison of the process name against the 
// ProcessName parameter. The function will return 0 on failure.
inline DWORD GetProcessIdFromName(LPCTSTR ProcessName)
{
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = NULL;
	ZeroMemory(&pe32, sizeof(PROCESSENTRY32));

	// We want a snapshot of processes
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Check for a valid handle, in this case we need to check for
	// INVALID_HANDLE_VALUE instead of NULL
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	// Now we can enumerate the running process, also 
	// we can't forget to set the PROCESSENTRY32.dwSize member
	// otherwise the following functions will fail
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &pe32) == FALSE)
	{
		// Cleanup the mess
		CloseHandle(hSnapshot);
		return 0;
	}

	// Do our first comparison
	if (_tcsicmp(pe32.szExeFile, ProcessName) == FALSE)
	{
		// Cleanup the mess
		CloseHandle(hSnapshot);
		return pe32.th32ProcessID;
	}

	// Most likely it won't match on the first try so 
	// we loop through the rest of the entries until
	// we find the matching entry or not one at all
	while (Process32Next(hSnapshot, &pe32))
	{
		if (_tcsicmp(pe32.szExeFile, ProcessName) == 0)
		{
			// Cleanup the mess
			CloseHandle(hSnapshot);
			return pe32.th32ProcessID;
		}
	}

	// If we made it this far there wasn't a match
	// so we'll return 0
	CloseHandle(hSnapshot);
	return 0;
}

// This function uses the toolhelp32 api to enumerate all running processes
// on the computer and does a comparison of the process name against the 
// ProcessName parameter. The function will return 0 on failure.
inline DWORD GetProcessIdFromName(std::string procname)
{
	std::wstring ProcessName(procname.begin(), procname.end());

	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = NULL;
	ZeroMemory(&pe32, sizeof(PROCESSENTRY32));

	// We want a snapshot of processes
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Check for a valid handle, in this case we need to check for
	// INVALID_HANDLE_VALUE instead of NULL
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	// Now we can enumerate the running process, also 
	// we can't forget to set the PROCESSENTRY32.dwSize member
	// otherwise the following functions will fail
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &pe32) == FALSE)
	{
		// Cleanup the mess
		CloseHandle(hSnapshot);
		return 0;
	}

	// Do our first comparison
	if (_tcsicmp(pe32.szExeFile, ProcessName.c_str()) == FALSE)
	{
		// Cleanup the mess
		CloseHandle(hSnapshot);
		return pe32.th32ProcessID;
	}

	// Most likely it won't match on the first try so 
	// we loop through the rest of the entries until
	// we find the matching entry or not one at all
	while (Process32Next(hSnapshot, &pe32))
	{
		if (_tcsicmp(pe32.szExeFile, ProcessName.c_str()) == 0)
		{
			// Cleanup the mess
			CloseHandle(hSnapshot);
			return pe32.th32ProcessID;
		}
	}

	// If we made it this far there wasn't a match
	// so we'll return 0
	CloseHandle(hSnapshot);
	return 0;
}

// This function will return the process id of csrss.exe
// and will do so in two different ways. If the OS is XP or 
// greater NtDll has a CsrGetProcessId otherwise I'll use 
// GetProcessIdFromName. Like other functions it will 
// return 0 on failure.

#pragma warning(disable : 4996)
inline DWORD GetCsrssProcessId()
{
	// Don't forget to set dw.Size to the appropriate
	// size (either OSVERSIONINFO or OSVERSIONINFOEX)
	OSVERSIONINFO osinfo;
	ZeroMemory(&osinfo, sizeof(OSVERSIONINFO));
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// Shouldn't fail
	GetVersionEx(&osinfo);

	// Visit http://msdn.microsoft.com/en-us/library/ms724833(VS.85).aspx
	// for a full table of versions however what I have set will
	// trigger on anything XP and newer including Server 2003
	if (osinfo.dwMajorVersion >= 5 && osinfo.dwMinorVersion >= 1)
	{
		// Gotta love functions pointers
		typedef DWORD(__stdcall *pCsrGetId)();

		// Grab the export from NtDll
		pCsrGetId CsrGetProcessId = (pCsrGetId)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "CsrGetProcessId");

		if (CsrGetProcessId)
			return CsrGetProcessId();
		else
			return 0;
	}
	else
		return GetProcessIdFromName(TEXT("csrss.exe"));
}

inline bool CanOpenCsrss()
{
	HANDLE Csrss = 0;

	// If we're being debugged and the process has
	// SeDebugPrivileges privileges then this call
	// will be successful, note that this only works
	// with PROCESS_ALL_ACCESS.

	Csrss = OpenProcess(PROCESS_ALL_ACCESS,
		FALSE, GetCsrssProcessId());

	if (Csrss != NULL)
	{
		CloseHandle(Csrss);
		return true;
	}
	else
		return false;
}

// GetParentProcessId will use the NtQueryInformationProcess function
// exported by NtDll to retrieve the parent process id for the current 
// process and if for some reason it doesn't work, it returns 0
inline DWORD GetParentProcessId()
{
	// Much easier in ASM but C/C++ looks so much better
	typedef NTSTATUS(WINAPI *pNtQueryInformationProcess)
		(HANDLE, UINT, PVOID, ULONG, PULONG);

	// Some locals
	NTSTATUS Status = 0;
	PROCESS_BASIC_INFORMATION pbi;
	ZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION));

	// Get NtQueryInformationProcess
	pNtQueryInformationProcess NtQIP = (pNtQueryInformationProcess)
		GetProcAddress(
			GetModuleHandle(TEXT("ntdll.dll")),
			"NtQueryInformationProcess");

	// Sanity check although there's no reason for it to have failed
	if (NtQIP == 0)
		return 0;

	// Now we can call NtQueryInformationProcess, the second
	// param 0 == ProcessBasicInformation
	Status = NtQIP(GetCurrentProcess(), 0, (void*)&pbi,
		sizeof(PROCESS_BASIC_INFORMATION), 0);

	if (Status != 0x00000000)
		return 0;
	else
		return pbi.ParentProcessId;
}

// Debug self is a function that uses CreateProcess
// to create an identical copy of the current process
// and debugs it
inline void DebugSelf()
{
	HANDLE hProcess = NULL;
	DEBUG_EVENT de;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&de, sizeof(DEBUG_EVENT));

	GetStartupInfo(&si);

	// Create the copy of ourself
	CreateProcess(NULL, GetCommandLine(), NULL, NULL, FALSE,
		DEBUG_PROCESS, NULL, NULL, &si, &pi);

	// Continue execution
	ContinueDebugEvent(pi.dwProcessId, pi.dwThreadId, DBG_CONTINUE);

	// Wait for an event
	WaitForDebugEvent(&de, INFINITE);
}

// ObjectListCheck uses NtQueryObject to check the environments
// list of objects and more specifically for the number of
// debug objects. This function can cause an exception (although rarely)
// so either surround it in a try catch or __try __except block
// but that shouldn't happen unless one tinkers with the function
inline bool ObjectListCheck()
{
	typedef NTSTATUS(NTAPI *pNtQueryObject)
		(HANDLE, UINT, PVOID, ULONG, PULONG);

	POBJECT_ALL_INFORMATION pObjectAllInfo = NULL;
	void *pMemory = NULL;
	NTSTATUS Status;
	unsigned long Size = 0;

	// Get NtQueryObject
	pNtQueryObject NtQO = (pNtQueryObject)GetProcAddress(
		GetModuleHandle(TEXT("ntdll.dll")),
		"NtQueryObject");

	// Get the size of the list
	Status = NtQO(NULL, 3, //ObjectAllTypesInformation
		&Size, 4, &Size);

	// Allocate room for the list
	pMemory = VirtualAlloc(NULL, Size, MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE);

	if (pMemory == NULL)
		return false;

	// Now we can actually retrieve the list
	Status = NtQO((HANDLE)-1, 3, pMemory, Size, NULL);

	// Status != STATUS_SUCCESS
	if (Status != 0x00000000)
	{
		VirtualFree(pMemory, 0, MEM_RELEASE);
		return false;
	}

	// We have the information we need
	pObjectAllInfo = (POBJECT_ALL_INFORMATION)pMemory;

	unsigned char *pObjInfoLocation =
		(unsigned char*)pObjectAllInfo->ObjectTypeInformation;

	ULONG NumObjects = pObjectAllInfo->NumberOfObjects;

	for (UINT i = 0; i < NumObjects; i++)
	{

		POBJECT_TYPE_INFORMATION pObjectTypeInfo =
			(POBJECT_TYPE_INFORMATION)pObjInfoLocation;

		// The debug object will always be present
		if (wcscmp(L"DebugObject", pObjectTypeInfo->TypeName.Buffer) == 0)
		{
			// Are there any objects?
			if (pObjectTypeInfo->TotalNumberOfObjects > 0)
			{
				VirtualFree(pMemory, 0, MEM_RELEASE);
				return true;
			}
			else
			{
				VirtualFree(pMemory, 0, MEM_RELEASE);
				return false;
			}
		}

		// Get the address of the current entries
		// string so we can find the end
		pObjInfoLocation =
			(unsigned char*)pObjectTypeInfo->TypeName.Buffer;

		// Add the size
		pObjInfoLocation +=
			pObjectTypeInfo->TypeName.Length;

		// Skip the trailing null and alignment bytes
		ULONG tmp = ((ULONG)pObjInfoLocation) & -4;

		// Not pretty but it works
		pObjInfoLocation = ((unsigned char*)tmp) +
			sizeof(unsigned long);
	}

	VirtualFree(pMemory, 0, MEM_RELEASE);
	return true;
}

inline bool IsDbgPresentPrefixCheck()
{
	__try
	{
		__asm __emit 0xF3 // 0xF3 0x64 disassembles as PREFIX REP:
		__asm __emit 0x64
		__asm __emit 0xF1 // One byte INT 1
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

inline bool checkGlobalFlags()
{
	DWORD NtGlobalFlags;
	__asm
	{
		mov eax, fs:[30h]
		mov eax, [eax + 68h]
		mov NtGlobalFlags, eax
	}

	if (NtGlobalFlags & 0x70)
		return true;

	return false;
}

inline bool debug1()
{
	if (IsDebuggerPresent())
	{
		DBMB("IsDebuggerPresent");
		restore(DEBUG1);
		ExitProcess(0);
	}
	return false;
}

inline bool debug2()
{
	char IsDbgPresent = 0;
	__asm {
		mov eax, fs:[30h]
		mov al, [eax + 2h]
		mov IsDbgPresent, al
	}

	if (IsDbgPresent)
	{
		DBMB("IsDbgPresent");
		restore(DEBUG2);
		ExitProcess(0);
	}
	return false;
}

inline bool debug3()
{
	BOOL IsDbgPresent = FALSE;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &IsDbgPresent);
	if (IsDbgPresent)
	{
		DBMB("CheckRemoteDebuggerPresent");
		restore(DEBUG3);
		ExitProcess(0);
	}

	return false;
}//

inline bool debug4()
{
	// Function Pointer Typedef for NtQueryInformationProcess
	typedef unsigned long(__stdcall *pfnNtQueryInformationProcess)(IN  HANDLE,
		IN  unsigned int, OUT PVOID, IN ULONG, OUT PULONG);


	// ProcessDebugPort
	const int ProcessDbgPort = 7;

	// We have to import the function
	pfnNtQueryInformationProcess NtQueryInfoProcess = NULL;

	// Other Vars
	unsigned long Ret;
	unsigned long IsRemotePresent = 0;

	HMODULE hNtDll = LoadLibrary(TEXT("ntdll.dll"));
	if (hNtDll == NULL)
	{
		// Handle however.. chances of this failing
		// is essentially 0 however since
		// ntdll.dll is a vital system resource
	}

	NtQueryInfoProcess = (pfnNtQueryInformationProcess)GetProcAddress(hNtDll, "NtQueryInformationProcess");
	if (NtQueryInfoProcess == NULL)
	{
		// Handle however it fits your needs but as before,
		// if this is missing there are some SERIOUS issues with the OS
	}

	// Time to finally make the call
	Ret = NtQueryInfoProcess(GetCurrentProcess(), ProcessDbgPort, &IsRemotePresent, sizeof(unsigned long), nullptr);
	if (Ret == 0x00000000 && IsRemotePresent != 0)
	{
		// Debugger is present
		DBMB("debug4");
		restore(DEBUG4);
		ExitProcess(0);
	}

	return false;
}//

inline bool debug5()
{
	unsigned long NtGlobalFlags = 0;

	__asm {

		mov eax, fs:[30h]
		mov eax, [eax + 68h]
		mov NtGlobalFlags, eax
	}


	if (NtGlobalFlags & 0x70)
		// 0x70 =  FLG_HEAP_ENABLE_TAIL_CHECK |
		//         FLG_HEAP_ENABLE_FREE_CHECK | 
		//         FLG_HEAP_VALIDATE_PARAMETERS
	{
		// Debugger is present
		DBMB("debug5");
		restore(DEBUG5);
		ExitProcess(0);
	}

	return false;
}//

#define JUNK_CODE_ONE        \
    __asm{push eax}            \
    __asm{xor eax, eax}        \
    __asm{setpo al}            \
    __asm{push edx}            \
    __asm{xor edx, eax}        \
    __asm{sal edx, 2}        \
    __asm{xchg eax, edx}    \
    __asm{pop edx}            \
    __asm{or eax, ecx}        \
    __asm{pop eax}

inline int AddSubOne(int One, int Two)
{
	JUNK_CODE_ONE
		return ((One + Two) - 1);
}

inline bool checkDebugger()
{
	debug1();
	debug2();
	debug3();
	debug4();
	debug5();

	//olly stuff
	HANDLE hOlly = FindWindow(TEXT("OLLYDBG"), NULL);

	if (hOlly)
	{
		DBMB("hOlly");
		restore(DEBUGGER_OLLY);
		ExitProcess(0);
	}
		

	OutputDebugString(TEXT("%s%s%s%s%s%s%s%s%s%s%s")
		TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s")
		TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s")
		TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s"));

	//windbg stuff
	HANDLE hWinDbg = FindWindow(TEXT("WinDbgFrameClass"), NULL);

	if (hWinDbg)
	{
		DBMB("hWinDbg");
		restore(DEBUGGER_WINDBG);
		ExitProcess(0);
	}

	return false;
}

inline void doChecks()
{
	//crashes
	//if (Int2DCheck())
	//{
	//	MessageBoxA(NULL, "1111", "", MB_OK);
	//	Hooks::restore(INT2DCHECK);
	//	ExitThread(0);
	//}

	BADPROCESS(XorStr("cheatengine"));
	BADPROCESS(XorStr("fiddler"));
	BADPROCESS(XorStr("olly"));
	BADPROCESS(XorStr("dnspy"));
	BADPROCESS(XorStr("idaq"));
	BADPROCESS(XorStr("procexp"));
	BADPROCESS(XorStr("reclass"));

	Sleep(120);

	AddSubOne(1, 2);

	Sleep(120);

	//check for debuggers
	checkDebugger();

	Sleep(120);
	try
	{
		ObjectListCheck();
	} catch (int e)
	{
		DBMB("ObjectListCheck");
		restore(OBJECTLISTCHECK);
		ExitThread(e);
	}

	Sleep(120);
	PushPopSS();

	//GenerateSerialWin32Attack(); //can exit the program if a function takes too long to execute

	Sleep(120);

	if (CheckHardwareBreakpoints())
	{
		DBMB("CheckHardwareBreakpoints");
		restore(CHECKHARDWAREBREAKPOINTS);
		ExitThread(0);
	}
	Sleep(120);
	if (CheckProcessDebugFlags())
	{
		DBMB("CheckProcessDebugFlags");
		restore(CHECKPROCESSDEBUGFLAGS);
		ExitThread(0);
	}
	Sleep(120);
	if (DebugObjectCheck())
	{
		DBMB("DebugObjectCheck");
		restore(DEBUGOBJECTCHECK);
		ExitThread(0);
	}
	Sleep(120);
	/*if (CheckOutputDebugString(L"nice"))
	{
		MessageBoxA(NULL, "CheckOutputDebugString", "", MB_OK);
		Hooks::restore(CHECKOUTPUTDEBUGSTRING);
		ExitThread(0);
	}*/
	Sleep(120);
	/*if (CanOpenCsrss())
	{
		MessageBoxA(NULL, "CanOpenCsrss", "", MB_OK);
		restore(CANOPENCSRSS);
		ExitThread(0);
	}
	Sleep(120);*/

	AddSubOne(3, 4);
	Sleep(120);

	//if (IsDbgPresentPrefixCheck())
	//{
	//	Hooks::restore(ISDBGPRESENTPREFIXCHECK);
	//	ExitThread(0);
	//}

	AddSubOne(5, 6);
	Sleep(120);

	if (checkGlobalFlags())
	{
		DBMB("checkGlobalFlags");
		restore(CHECKGLOBALFLAGS);
	}
}

inline void securityThread(LPARAM lpParam)
{
	while (true)
	{
		doChecks();
		Sleep(5000);
	}
}

inline void initSecurity()
{
#ifndef DEV_ENABLED
	VIRTUALIZER_FISH_WHITE_START
#endif
	if (!ValidHDDSize())
	{
		DBMB("ValidHDDSize");
		restore(VALIDHDDSIZE);
		ExitThread(0);
	}
	if (swallow_redpill())
	{
		DBMB("swallow_redpill");
		restore(SWALLOW_REDPILL);
		ExitThread(0);
	}
	if (IsInsideVPC())
	{
		DBMB("IsInsideVPC");
		restore(ISINSIDEVPC);
		ExitThread(0);
	}
	if (IsInsideVMWare())
	{
		DBMB("IsInsideVMWare");
		restore(ISINSIDEVMWARE);
		ExitThread(0);
	}

	if (MemoryBreakpointDebuggerCheck())
	{
		DBMB("MemoryBreakpointDebuggerCheck");
		restore(MEMORYBREAKPOINTDEBUGGERCHECK);
		ExitThread(0);
	}

	DebugSelf();
	
	HideThread(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)securityThread, NULL, NULL, NULL));

#ifndef DEV_ENABLED
	VIRTUALIZER_FISH_WHITE_END
#endif
}


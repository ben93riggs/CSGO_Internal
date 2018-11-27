#pragma once

class IGameEvent
{
public:
	virtual					~IGameEvent() {};
	virtual const char*		GetName() const = 0;

	virtual bool			IsReliable() const = 0;
	virtual bool			IsLocal() const = 0;
	virtual bool			IsEmpty(const char* keyname = nullptr) = 0;

	virtual bool			GetBool(const char* keyname = nullptr, bool default_value = false) = 0;
	virtual int				GetInt(const char* keyname = nullptr, int default_value = 0) = 0;
	virtual uint64_t		GetUint64(const char* keyname = nullptr, uint64_t default_value = 0) = 0;
	virtual float			GetFloat(const char* keyname = nullptr, float default_value = 0.0f) = 0;
	virtual const char*		GetString(const char* keyname = nullptr, const char* default_value = "") = 0;
	virtual const wchar_t*	GetWString(const char* keyname = nullptr, const wchar_t* default_value = L"") = 0;
	virtual const void*		GetPtr(const char* keyname = nullptr, const void* default_values = nullptr) = 0;

	virtual void			SetBool(const char* keyname, bool value) = 0;
	virtual void			SetInt(const char* keyname, int value) = 0;
	virtual void			SetUint64(const char* keyname, uint64_t value) = 0;
	virtual void			SetFloat(const char* keyname, float value) = 0;
	virtual void			SetString(const char* keyname, const char* value) = 0;
	virtual void			SetWString(const char* keyname, const wchar_t* value) = 0;
	virtual void			SetPtr(const char* keyname, const void* value) = 0;
};

class IGameEventListener2 
{
public:
	virtual ~IGameEventListener2() {};
	virtual void FireGameEvent(IGameEvent* Event) = 0;
	virtual int GetEventDebugID() = 0;
};

class IGameEventManager2 
{
public:
	virtual ~IGameEventManager2(void) {};
	virtual int LoadEventsFromFile(const char* filename) = 0;
	virtual void Reset() = 0;
	virtual bool AddListener(IGameEventListener2* listener, const char* name, bool bServerSide) = 0;
	virtual bool FindListener(IGameEventListener2* listener, const char* name) = 0;
	virtual void RemoveListener(IGameEventListener2* listener) = 0;
	virtual void AddListenerGlobal(IGameEventListener2* listener, bool bIsServerSide) = 0;
	virtual IGameEvent* CreateGameEvent(const char* name, bool bForce = false, int* pCookie = NULL) = 0;
	virtual bool FireEvent(IGameEvent* event, bool bDontBroadcast = false) = 0;
	virtual bool FireEventClientSide(IGameEvent* event) = 0;
	virtual IGameEvent* DuplicateEvent(IGameEvent* event) = 0;
	virtual void FreeEvent(IGameEvent* event) = 0;
	virtual bool SerializeEvent(IGameEvent* event, void* buf) = 0;
	virtual IGameEvent* UnserializeEvent(void* buf) = 0;
	virtual void *GetEventDataTypes(IGameEvent* event) = 0;
};
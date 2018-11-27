#pragma once
#include <memory>
#include <unordered_map>
#include "SourceEngine/Recv.hpp"
#include <map>
#include <cassert>

//Forward declaration. The class is defined on the cpp file
struct NetvarTable;
class NetvarDatabase;

using fnv_t = unsigned;

class FnvHash
{
	static const fnv_t FNV_PRIME = 16777619u;
	static const fnv_t OFFSET_BASIS = 2166136261u;

	template <unsigned int N>
	static constexpr fnv_t fnvHashConst(const char(&str)[N], unsigned int I = N)
	{
		return static_cast<fnv_t>(1ULL * (I == 1 ? (OFFSET_BASIS ^ str[0]) : (fnvHashConst(str, I - 1) ^ str[I - 1])) * FNV_PRIME);
	}

	static fnv_t fnvHash(const char* str)
	{
		const auto length = strlen(str) + 1;
		auto hash = OFFSET_BASIS;
		for (size_t i = 0; i < length; ++i)
		{
			hash ^= *str++;
			hash *= FNV_PRIME;
		}
		return hash;
	}

	struct Wrapper
	{
		Wrapper(const char* str) : str(str) { }
		const char* str;
	};

	fnv_t hash_value;
public:
	FnvHash(Wrapper wrapper) : hash_value(fnvHash(wrapper.str)) { }

	template <unsigned int N>
	constexpr FnvHash(const char(&str)[N]) : hash_value(fnvHashConst(str)) { }

	constexpr operator fnv_t() const { return this->hash_value; }
};

class NetvarManager {
private:
     static NetvarManager* instance;

     NetvarManager();
     ~NetvarManager();

     NetvarManager(const NetvarManager&) = delete;

public:
     static NetvarManager* Instance() {
          if(!instance) instance = new NetvarManager;
          return instance;
     }

     void CreateDatabase();
     void DestroyDatabase();

	//////////////////////
     void Dump(std::ostream& stream) const;
	static RecvTable* GetTable(std::vector<RecvTable*> tables, const char* tableName);
	int GetProp2(std::vector<RecvTable*> tables, const char* tableName, const char* propName, RecvProp** prop) const;
	int GetProp2(std::vector<RecvTable*> tables, RecvTable* recvTable, const char* propName, RecvProp** prop) const;
	static std::vector<RecvTable*> GetTables();
	bool HookProp(const char* tableName, const char* propName, RecvVarProxyFn f) const;
	////////////////////


	void Dump(const std::string& file) const;

     int GetNetvarCount() const { return m_netvarCount; }
     int GetTableCount() const { return m_tableCount; }

     template<typename ...Args> 
     uint32_t GetOffset(const std::string& szTableName, Args&&... args) {
          return GetOffset(szTableName, {std::forward<Args>(args)...});
     }

	 uint32_t GetOffset(const std::string& szTableName, const std::initializer_list<std::string>& props) const;

	 template<typename ...Args>
	 static void HookVar(RecvVarProxyFn& OriginalFunction, void* Hooked_Func, const std::string& szTableName, Args&&... args)
	 {
		 HookVar(OriginalFunction, Hooked_Func, szTableName, { std::forward<Args>(args)... });
	 }

	static void HookVar(RecvVarProxyFn& OriginalFunction, void* Hooked_Func, const std::string& szTableName, const std::initializer_list<std::string>& props);
	static void UnhookRecvProxies();

private:
     std::unique_ptr<NetvarTable> InternalLoadTable(RecvTable* pRecvTable, uint32_t offset);
     void Dump(std::ostream& output, NetvarTable& table, int level) const;

     std::unique_ptr<NetvarDatabase>    m_pDatabase    = nullptr;
     uint32_t                           m_tableCount   = 0;
     uint32_t                           m_netvarCount  = 0;
};


#define GET_NETVAR(table, ...) NetvarManager::Instance()->GetOffset(table, __VA_ARGS__)
#define HOOK_NETVAR(OriginalFunction, Hooked_Func, table, ...) NetvarManager::HookVar(OriginalFunction, Hooked_Func, table, __VA_ARGS__)

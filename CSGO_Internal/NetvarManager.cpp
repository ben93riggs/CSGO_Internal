#include "NetvarManager.hpp"

#include <fstream>
#include <utility>
#include "SourceEngine/SDK.hpp"
#include <iostream>


using namespace std;


//------------------------------------------------------------
// Classes to help the parsing of the netvars
//------------------------------------------------------------
#pragma region Helper classes

typedef unordered_map<string, unique_ptr<NetvarTable>>           TableMap;
typedef unordered_map<string, uint32_t>                          PropMap;

typedef unordered_map<string, unique_ptr<NetvarTable>>::iterator Iter;

struct NetvarTable {
     TableMap  m_ChildTables;
     PropMap   m_ChildProps;
     uint32_t  m_uOffset  = 0;

     bool is_empty() const
     {
          return m_ChildTables.size() == 0 && m_ChildProps.size() == 0;
     }
     void insert_table(string&& name, unique_ptr<NetvarTable>&& pTable) {
          m_ChildTables.emplace(name, move(pTable));
     }
     void insert_prop(string&& name, uint32_t offset) {
          m_ChildProps.emplace(name, offset);
     }
};

class NetvarDatabase {
public:
     TableMap m_Tables;

     void insert(string&& name, unique_ptr<NetvarTable>&& pTable) {
          m_Tables.emplace(name, move(pTable));
     }
     Iter find(const string& key)  { return m_Tables.find(key); }
     Iter begin()                  { return m_Tables.begin(); }
     Iter end()                    { return m_Tables.end(); }

};

#pragma endregion

//------------------------------------------------------------
// Netvar Manager
//------------------------------------------------------------
NetvarManager* NetvarManager::instance = nullptr;

NetvarManager::NetvarManager() {

}

NetvarManager::~NetvarManager() {

}

void NetvarManager::CreateDatabase() {
     m_pDatabase = make_unique<NetvarDatabase>();
     auto pClient = Interface->Client();

     if(pClient) {
          for( auto pClass = pClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) 
          {
               if(pClass->m_pRecvTable) 
               {
                    //Insert new entry on the database
                    m_pDatabase->insert(pClass->m_pRecvTable->m_pNetTableName, InternalLoadTable(pClass->m_pRecvTable, 0));
                    m_tableCount++;
               }
          }
     }
}

void NetvarManager::Dump(ostream& output) const
{
     for(auto& entry : *m_pDatabase) {
          auto& pTable = entry.second;
          if(pTable->is_empty())
               continue;
          output << entry.first << endl;
          Dump(output, *pTable, 1);
          output << XorStr("==============================================================") << endl;
     }
}

RecvTable* NetvarManager::GetTable(vector<RecvTable*> tables, const char* tableName)
{
	if (tables.empty())
		return nullptr;

	for (unsigned long i = 0; i < tables.size(); i++)
	{
		RecvTable* table = tables[i];

		if (!table)
			continue;

		if (strcmp(table->m_pNetTableName, tableName) == 0)
			return table;
	}

	return nullptr;
}

int NetvarManager::GetProp2(vector<RecvTable*> tables, const char* tableName, const char* propName, RecvProp** prop) const
{
	RecvTable* recvTable = GetTable(tables, tableName);
	if (!recvTable)
		return 0;

	int offset = GetProp2(tables, recvTable, propName, prop);
	if (!offset)
		return 0;

	return offset;
}

int NetvarManager::GetProp2(vector<RecvTable*> tables, RecvTable* recvTable, const char* propName, RecvProp** prop) const
{
	int extraOffset = 0;

	for (int i = 0; i < recvTable->m_nProps; ++i) {
		RecvProp* recvProp = &recvTable->m_pProps[i];
		RecvTable* child = recvProp->m_pDataTable;

		if (child && (child->m_nProps > 0))
		{
			int tmp = GetProp2(tables, child, propName, prop);
			if (tmp)
				extraOffset += (recvProp->m_Offset + tmp);
		}

		if (strcmp(recvProp->m_pVarName, propName))
			continue;

		if (prop)
			*prop = recvProp;

		return (recvProp->m_Offset + extraOffset);
	}

	return extraOffset;
}

vector<RecvTable*> NetvarManager::GetTables()
{
	vector<RecvTable*> tables;

	ClientClass* clientClass = CLIENT->GetAllClasses();
	if (!clientClass)
		return vector<RecvTable*>();

	while (clientClass)
	{
		RecvTable* recvTable = clientClass->m_pRecvTable;
		tables.push_back(recvTable);

		clientClass = clientClass->m_pNext;
	}

	return tables;
}

bool NetvarManager::HookProp(const char* tableName, const char* propName, RecvVarProxyFn f) const
{
	RecvProp* recvProp = nullptr;
	GetProp2(GetTables(), tableName, propName, &recvProp);
	if (!recvProp)
		return false;

	recvProp->m_ProxyFn = f;

	return true;
}

void NetvarManager::Dump(const string& file) const
{
     ofstream output(file);
     Dump(output);
}

//------------------------------------------------------------
// Internal methods below. This is where the real work is done
//------------------------------------------------------------
unique_ptr<NetvarTable> NetvarManager::InternalLoadTable(RecvTable* pRecvTable, uint32_t offset)
{
	auto pTable = make_unique<NetvarTable>();
	pTable->m_uOffset = offset;

	for( auto i = 0;  i < pRecvTable->m_nProps; ++i) 
	{
		auto pProp = &pRecvTable->m_pProps[i];

		//Skip trash array items
		if(!pProp || isdigit(pProp->m_pVarName[0])) continue;
		//We dont care about the base class
		if(strcmp(pProp->m_pVarName, XorStr("baseclass")) == 0) continue;

		//If this prop is a table
		if(pProp->m_RecvType == static_cast<int>(SendPropType::DPT_DataTable)	//If it is a table AND
			&& pProp->m_pDataTable != nullptr									//The DataTable isnt null AND
			&& pProp->m_pDataTable->m_pNetTableName[0] == 'D')					//The Table name starts with D (this is because there are some crappy nested 
		{																		//tables that we want to skip, and those dont start with D)
			//load the table pointed by pProp->m_pDataTable and insert it
			pTable->insert_table(pProp->m_pVarName, InternalLoadTable(pProp->m_pDataTable, pProp->m_Offset));
		} else { 
			pTable->insert_prop(pProp->m_pVarName, pProp->m_Offset);
		}
		m_netvarCount++;
     }
     return pTable;
}

void NetvarManager::Dump(ostream& output, NetvarTable& table, int level) const
{
     char line[512] = "";
     char fmt[512] = "";

     //yo dawg, i hear u like formatting strings so i am formatting ur formatting string
     sprintf(fmt, "%%-%ds: 0x%%08X\n", 50 - level * 4);

     for(auto& prop : table.m_ChildProps) {
          for(int i = 0; i < level; i++) {
               if(i != level - 1) {
                    output << "    ";
               } else {
                    output << "|___";
               }
          }
          sprintf(line, fmt, prop.first.c_str(), prop.second + table.m_uOffset);
          output << line;

     }
     for(auto& child : table.m_ChildTables) {
          for(int i = 0; i < level; i++) {
               if(i != level - 1) {
                    output << "    ";
               } else {
                    output << "|___";
               }
          }
          sprintf(line, fmt, child.first.c_str(), child.second->m_uOffset);
          output << line;
          Dump(output, *child.second, level + 1);
     }
}

uint32_t NetvarManager::GetOffset(const string& szTableName, const initializer_list<string>& props) const
{
     auto table = m_pDatabase->find(szTableName);
     if(table == m_pDatabase->end()) return -1;

     int tableOffset = table->second->m_uOffset;
     if(props.size() == 0) return tableOffset;

     int totalOffset = tableOffset;

     NetvarTable* curTable = table->second.get();

     for(int i = 0; i < props.size(); i++) 
	 {
          string propName = *(props.begin() + i);

		if(i + 1 < props.size()) //This index is not the last one
		{
			auto childTable = curTable->m_ChildTables.find(propName);

			if(childTable == curTable->m_ChildTables.end()) 
			{
				throw runtime_error("Prop not found");
			}

			totalOffset += childTable->second->m_uOffset;

			curTable = childTable->second.get();
		} 
		else //Last index, retrieve prop instead of table
		{ 
			auto childProp = curTable->m_ChildProps.find(propName);

			if (childProp == curTable->m_ChildProps.end()) 
			{
				throw runtime_error("Prop not found");
			}

			totalOffset += childProp->second;
          }
     }

     return totalOffset;
}

multimap<void*, pair<string, vector<string>>> HookedNetvars;

void NetvarManager::HookVar(RecvVarProxyFn& OriginalFunction, void* Hooked_Func, const string& szTableName, const initializer_list<string>& props)
{
	void* originalPtr;
	vector<string> proplist;

	ClientClass *pClass = CLIENT->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;

		if (!strcmp(pszName, szTableName.c_str()))
		{
			for (int i = 0; i < props.size(); i++)
			{
				string propName = *(props.begin() + i);
				proplist.push_back(propName);

				for (int k = 0; k < pClass->m_pRecvTable->m_nProps; k++)
				{
					RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[k]);
					const char *name = pProp->m_pVarName;

					if (!strcmp(name, propName.c_str()))
					{
						OriginalFunction = static_cast<RecvVarProxyFn>(pProp->m_ProxyFn);
						originalPtr = pProp->m_ProxyFn;
						pProp->m_ProxyFn = Hooked_Func;
					}
				}
			}
		}

		pClass = pClass->m_pNext;
	}

	HookedNetvars.emplace(originalPtr, make_pair(szTableName, proplist));
}



void NetvarManager::UnhookRecvProxies()
{
	for each (auto netvar in HookedNetvars)
	{
		void* original = netvar.first;
		string szTableName = netvar.second.first;
		vector<string> props = netvar.second.second;

		ClientClass *pClass = CLIENT->GetAllClasses();
		while (pClass)
		{
			const char *pszName = pClass->m_pRecvTable->m_pNetTableName;

			if (!strcmp(pszName, szTableName.c_str()))
			{
				for (int i = 0; i < props.size(); i++)
				{
					string propName = *(props.begin() + i);

					for (int k = 0; k < pClass->m_pRecvTable->m_nProps; k++)
					{
						RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[k]);
						const char *name = pProp->m_pVarName;

						if (!strcmp(name, propName.c_str()))
						{
							pProp->m_ProxyFn = original;
						}
					}
				}
			}

			pClass = pClass->m_pNext;
		}


	}
}

#pragma once
#include <Windows.h>
#include <map>

namespace hooks
{
	inline bool IsCodePtr(void* ptr)
	{
		constexpr const DWORD protect_flags = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

		MEMORY_BASIC_INFORMATION out;
		VirtualQuery(ptr, &out, sizeof out);

		return out.Type
			&& !(out.Protect & (PAGE_GUARD | PAGE_NOACCESS))
			&& out.Protect & protect_flags;
	}

	typedef DWORD** PPDWORD;
	class VFTableHook 
	{
		VFTableHook(const VFTableHook&) = delete;
	public:
		VFTableHook(PPDWORD ppClass, bool bReplace) 
		{
			try
			{
				m_ppClassBase = ppClass;
				m_bReplace = bReplace;
				if (bReplace) {
					m_pOriginalVMTable = *ppClass;
					uint32_t dwLength = CalculateLength();

					m_pNewVMTable = new DWORD[dwLength];
					memcpy(m_pNewVMTable, m_pOriginalVMTable, dwLength * sizeof(DWORD));

					DWORD old;
					VirtualProtect(m_ppClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
					*m_ppClassBase = m_pNewVMTable;
					VirtualProtect(m_ppClassBase, sizeof(DWORD), old, &old);

				}
				else 
				{
					m_pOriginalVMTable = *ppClass;
					m_pNewVMTable = *ppClass;
				}

			} 
			catch (int e)
			{
				//printf("Error while hooking: %d\n", e);
			}
		}
		~VFTableHook() 
		{
			RestoreTable();
			if (m_bReplace && m_pNewVMTable)
				delete[] m_pNewVMTable;
		}

		void RestoreTable() 
		{
			if (m_bReplace) 
			{
				DWORD old;
				VirtualProtect(m_ppClassBase, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
				*m_ppClassBase = m_pOriginalVMTable;
				VirtualProtect(m_ppClassBase, sizeof(DWORD), old, &old);
			}
			else 
			{
				for (auto& pair : m_vecHookedIndexes) 
				{
					Unhook(pair.first);
				}
			}
		}

		template<class Type>
		Type Hook(uint32_t index, Type fnNew) 
		{
			DWORD dwOld = (DWORD)m_pOriginalVMTable[index];
			m_pNewVMTable[index] = (DWORD)fnNew;
			m_vecHookedIndexes.insert(std::make_pair(index, (DWORD)dwOld));
			return (Type)dwOld;
		}
		void Unhook(uint32_t index) 
		{
			auto it = m_vecHookedIndexes.find(index);
			if (it != m_vecHookedIndexes.end()) 
			{
				m_pNewVMTable[index] = (DWORD)it->second;
				m_vecHookedIndexes.erase(it);
			}
		}

		template<class Type>
		Type GetOriginal(uint32_t index) 
		{
			return (Type)m_pOriginalVMTable[index];
		}

		template<class Type>
		static Type HookManual(uintptr_t* vftable, uint32_t index, Type fnNew)
		{
			DWORD dummy;
			const auto fn_old = (Type)vftable[index];
			VirtualProtect(static_cast<void*>(&vftable[index]), 0x4, PAGE_EXECUTE_READWRITE, &dummy);
			vftable[index] = (uintptr_t)fnNew;
			VirtualProtect(static_cast<void*>(&vftable[index]), 0x4, dummy, &dummy);
			return fn_old;
		}

	private:
		uint32_t CalculateLength() const
		{
			uint32_t dwIndex;
			if (!m_pOriginalVMTable) return 0;
			for (dwIndex = 0; m_pOriginalVMTable[dwIndex]; dwIndex++)
			{
				if (!IsCodePtr((FARPROC)m_pOriginalVMTable[dwIndex]))
				{
					break;
				}
			}
			return dwIndex;
		}

		std::map<uint32_t, DWORD> m_vecHookedIndexes;

		PPDWORD m_ppClassBase;
		PDWORD m_pOriginalVMTable;
		PDWORD m_pNewVMTable;
		bool m_bReplace;
	};
}
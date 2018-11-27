#include "NetvarHooks.h"
#include "SourceEngine/Recv.hpp"
#include "Globals.h"
#include "ItemDefinitions.h"
#include "fnv_hash.hpp"

RecvVarProxyFn	oRecvnSequence, oDidSmokeEffect,
				oSmokeEffectTickBegin, oObserverMode;

enum ESequence
{
	SEQUENCE_DEFAULT_DRAW = 0,
	SEQUENCE_DEFAULT_IDLE1 = 1,
	SEQUENCE_DEFAULT_IDLE2 = 2,
	SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
	SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
	SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
	SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
	SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
	SEQUENCE_DEFAULT_LOOKAT01 = 12,

	SEQUENCE_BUTTERFLY_DRAW = 0,
	SEQUENCE_BUTTERFLY_DRAW2 = 1,
	SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
	SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

	SEQUENCE_FALCHION_IDLE1 = 1,
	SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
	SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
	SEQUENCE_FALCHION_LOOKAT01 = 12,
	SEQUENCE_FALCHION_LOOKAT02 = 13,

	SEQUENCE_DAGGERS_IDLE1 = 1,
	SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
	SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
	SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
	SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

	SEQUENCE_BOWIE_IDLE1 = 1,
};

inline int random_sequence(int low, int high)
{
	return rand() % (high - low + 1) + low;
}

static auto fix_animation(const fnv::hash model, const int sequence) -> int
{
	enum ESequence
	{
		SEQUENCE_DEFAULT_DRAW = 0,
		SEQUENCE_DEFAULT_IDLE1 = 1,
		SEQUENCE_DEFAULT_IDLE2 = 2,
		SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
		SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
		SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
		SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
		SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
		SEQUENCE_DEFAULT_LOOKAT01 = 12,

		SEQUENCE_BUTTERFLY_DRAW = 0,
		SEQUENCE_BUTTERFLY_DRAW2 = 1,
		SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
		SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

		SEQUENCE_FALCHION_IDLE1 = 1,
		SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
		SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
		SEQUENCE_FALCHION_LOOKAT01 = 12,
		SEQUENCE_FALCHION_LOOKAT02 = 13,

		SEQUENCE_DAGGERS_IDLE1 = 1,
		SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
		SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
		SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
		SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

		SEQUENCE_BOWIE_IDLE1 = 1,
	};

	// Hashes for best performance.
	switch (model)
	{
	case FNV("models/weapons/v_knife_butterfly.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
		default:
			return sequence + 1;
		}
	}
	case FNV("models/weapons/v_knife_falchion_advanced.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	}
	case FNV("models/weapons/v_knife_push.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return random_sequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	}
	case FNV("models/weapons/v_knife_survival_bowie.mdl"):
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	}
	default:
		return sequence;
	}
}

void Hooked_RecvProxy_Sequence(CRecvProxyData *proxy_data_const, void *entity, void *output)
{
	if (!entity)
		return oRecvnSequence(proxy_data_const, entity, output);

	CBaseViewModel* pViewModel = static_cast<CBaseViewModel*>(entity);

	if (!pViewModel)
		return oRecvnSequence(proxy_data_const, entity, output);

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal || !pLocal->IsAlive())
		return oRecvnSequence(proxy_data_const, entity, output);

	// Remove the constness from the proxy data allowing us to make changes.
	const auto proxy_data = const_cast<CRecvProxyData*>(proxy_data_const);

	const auto view_model = static_cast<CBaseViewModel*>(entity);

	if (view_model && view_model->GetOwner() && view_model->GetOwner() != INVALID_EHANDLE_INDEX)
	{
		const auto owner = ENTITYLIST->GetClientEntityFromHandle(view_model->GetOwner());

		if (owner == pLocal)
		{
			const auto view_model_weapon_handle = view_model->GetWeapon();

			if (view_model_weapon_handle != INVALID_EHANDLE_INDEX)
			{
				const auto view_model_weapon = static_cast<CBaseAttributableItem*>(ENTITYLIST->GetClientEntityFromHandle(view_model_weapon_handle));

				if (view_model_weapon)
				{
					if (k_weapon_info.count(view_model_weapon->GetItemDefinitionIndex()))
					{
						auto original_sequence = proxy_data->m_Value.m_Int;

						const auto override_model = k_weapon_info.at(view_model_weapon->GetItemDefinitionIndex()).model;
						proxy_data->m_Value.m_Int = fix_animation(fnv::hash_runtime(override_model), proxy_data->m_Value.m_Int);
					}
				}
			}
			////////////////////////
			//// Get the filename of the current view model.
			//auto knife_model = MODELINFO->GetModel(view_model->GetModelIndex());
			//auto model_name = MODELINFO->GetModelName(knife_model);

			//if (animation_fix_map.count(model_name))
			//	proxy_data->m_Value.m_Int = animation_fix_map.at(model_name)(proxy_data->m_Value.m_Int);
		}
	}

	// Call original function with the modified data.
	oRecvnSequence(proxy_data, entity, output);
}

void Hooked_RecvProxy_DidSmokeEffect(CRecvProxyData *pData, IClientEntity* pStruct, void* pOut)
{
	if (!VisualVars::get().Removals.g_bNoSmoke)
		return oDidSmokeEffect(pData, pStruct, pOut);

	pData->m_Value.m_pData = reinterpret_cast<void*>(00000001); //value is stored as a bool (1 bit)

	oDidSmokeEffect(pData, pStruct, pOut);
}

void Hooked_RecvProxy_SmokeEffectTickBegin(CRecvProxyData *pData, IClientEntity* pStruct, void* pOut)
{
	if (!VisualVars::get().Removals.g_bNoSmoke)
		return oSmokeEffectTickBegin(pData, pStruct, pOut);

	pData->m_Value.m_Int = -1;

	oSmokeEffectTickBegin(pData, pStruct, pOut);
}

void RecvProxy_ObserverMode(CRecvProxyData *pData, IClientEntity* pStruct, void* pOut)
{
	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return oObserverMode(pData, pStruct, pOut);

	if (pLocal != pStruct)
		return oObserverMode(pData, pStruct, pOut);

	if (!pLocal->IsAlive() && MiscVars::get().g_bThirdPersonSpectator)
	{
		if (INPUT->m_fCameraInThirdPerson)
		{
			ENGINE->ExecuteClientCmd(XorStr("firstperson"));
		}
		pData->m_Value.m_Int = 5;
	}

	oObserverMode(pData, pStruct, pOut);
}

void HookRecvProxies()
{
	HOOK_NETVAR(oRecvnSequence, Hooked_RecvProxy_Sequence, XorStr("DT_BaseViewModel"), XorStr("m_nSequence"));
	HOOK_NETVAR(oSmokeEffectTickBegin, Hooked_RecvProxy_SmokeEffectTickBegin, XorStr("DT_SmokeGrenadeProjectile"), XorStr("m_nSmokeEffectTickBegin"));
	HOOK_NETVAR(oObserverMode, RecvProxy_ObserverMode, XorStr("DT_BasePlayer"), XorStr("m_iObserverMode"));
	HOOK_NETVAR(oDidSmokeEffect, Hooked_RecvProxy_DidSmokeEffect, XorStr("DT_SmokeGrenadeProjectile"), XorStr("m_bDidSmokeEffect"));
}
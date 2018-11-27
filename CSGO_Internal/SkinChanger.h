#pragma once
#include "SkinConfiguration.h"
#include "StickerChanger.h"

class SkinChanger : public singleton<SkinChanger>
{
	static void erase_override_if_exists_by_index(const int definition_index)
	{
		// We have info about the item not needed to be overridden
		if (k_weapon_info.count(definition_index))
		{
			auto& icon_override_map = g_config.get_icon_override_map();

			const auto& original_item = k_weapon_info.at(definition_index);

			// We are overriding its icon when not needed
			if (original_item.icon && icon_override_map.count(original_item.icon))
				icon_override_map.erase(icon_override_map.at(original_item.icon)); // Remove the leftover override
		}
	}

	static void apply_config_on_attributable_item(CBaseAttributableItem* item, const EconomyItem_t* config, unsigned xuid_low)
	{
		// Force fallback values to be used.
		item->GetItemIDHigh() = -1;

		// Set the owner of the weapon to our lower XUID. (fixes StatTrak)
		item->GetAccountID() = xuid_low;

		if (config->entity_quality_index)
			item->GetEntityQuality() = config->entity_quality_index;

		if (config->custom_name[0])
			strcpy_s(item->GetCustomName(), config->custom_name);

		if (config->paint_kit_index)
			item->GetFallbackPaintKit() = config->paint_kit_index;

		if (config->seed)
			item->GetFallbackSeed() = config->seed;

		if (config->stat_trak)
			item->GetFallbackStatTrak() = config->stat_trak;

		item->GetFallbackWear() = config->wear;

		auto& definition_index = item->GetItemDefinitionIndex();

		auto& icon_override_map = g_config.get_icon_override_map();

		if (config->definition_override_index // We need to override defindex
			&& config->definition_override_index != definition_index // It is not yet overridden
			&& k_weapon_info.count(config->definition_override_index)) // We have info about what we gonna override it to
		{
			const auto old_definition_index = definition_index;

			item->GetItemDefinitionIndex() = config->definition_override_index;

			const auto& replacement_item = k_weapon_info.at(config->definition_override_index);

			// Set the weapon model index -- required for paint kits to work on replacement items after the 29/11/2016 update.
			//item->GetModelIndex() = g_model_info->GetModelIndex(k_weapon_info.at(config->definition_override_index).model);
			item->SetModelIndex(MODELINFO->GetModelIndex(replacement_item.model));
			item->GetClientNetworkable()->PreDataUpdate(0);

			// We didn't override 0, but some actual weapon, that we have data for
			if (old_definition_index && k_weapon_info.count(old_definition_index))
			{
				const auto& original_item = k_weapon_info.at(old_definition_index);

				if (original_item.icon && replacement_item.icon)
					icon_override_map[original_item.icon] = replacement_item.icon;
			}
		}
		else
		{
			erase_override_if_exists_by_index(definition_index);
		}

		apply_sticker_changer(item);
	}

	static CreateClientClassFn get_wearable_create_fn()
	{
		auto clazz = CLIENT->GetAllClasses();

		while (strcmp(clazz->m_pNetworkName, XorStr("CEconWearable")))
			clazz = clazz->m_pNext;

		return clazz->m_pCreateFn;
	}

public:
	void FrameStageNotify(ClientFrameStage_t stage) const
	{
		if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START)
			return;

		const auto local_index = ENGINE->GetLocalPlayer();

		C_CSPlayer* pLocal = (C_CSPlayer*)ENTITYLIST->GetClientEntity(local_index);

		if (!pLocal)
			return;

		player_info_t player_info;
		if (!ENGINE->GetPlayerInfo(local_index, &player_info))
			return;

		#pragma region gloves
		 //Handle glove config
		{
			const auto wearables = pLocal->GetWearables();

			const auto glove_config = g_config.get_by_definition_index(GLOVE_T_SIDE);

			static CBaseHandle glove_handle = 0;

			auto glove = reinterpret_cast<CBaseAttributableItem*>(ENTITYLIST->GetClientEntityFromHandle(wearables[0]));

			if (!glove) // There is no glove
			{
				// Try to get our last created glove
				const auto our_glove = reinterpret_cast<CBaseAttributableItem*>(ENTITYLIST->GetClientEntityFromHandle(glove_handle));

				if (our_glove) // Our glove still exists
				{
					wearables[0] = glove_handle;
					glove = our_glove;
				}
			}

			if (!pLocal->IsAlive())
			{
				// We are dead but we have a glove, destroy it
				if (glove)
				{
					glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
					glove->GetClientNetworkable()->Release();
				}

				return;
			}

			if (glove_config && glove_config->definition_override_index)
			{
				// We don't have a glove, but we should
				if (!glove)
				{
					static auto create_wearable_fn = get_wearable_create_fn();

					const auto entry = ENTITYLIST->GetHighestEntityIndex() + 1;
					const auto serial = rand() % 0x1000;

					create_wearable_fn(entry, serial);
					glove = reinterpret_cast<CBaseAttributableItem*>(ENTITYLIST->GetClientEntity(entry));
					assert(glove);

					// He he
					{
						static auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>(Utils::FindPattern(Utils::get_client_dll(), (PBYTE)XorStr("\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1"), XorStr("xxxxxxxxxxxx")));

						static const Vector new_pos = { 10000.f, 10000.f, 10000.f };

						set_abs_origin_fn(glove, new_pos);
					}

					wearables[0] = entry | serial << 16;

					// Let's store it in case we somehow lose it.
					glove_handle = wearables[0];
				}

				// Thanks, Beakers
				glove->GetIndex() = -1;

				apply_config_on_attributable_item(glove, glove_config, player_info.m_nXuidLow);
			}
		}

		#pragma endregion

		#pragma region weapons
		//Handle weapon configs
		{
			auto& weapons = pLocal->GetWeapons();

			for (auto weapon_handle : weapons)
			{
				if (weapon_handle == INVALID_EHANDLE_INDEX)
					break;

				auto weapon = static_cast<CBaseAttributableItem*>(ENTITYLIST->GetClientEntityFromHandle(weapon_handle));

				if (!weapon)
					continue;

				auto& definition_index = weapon->GetItemDefinitionIndex();

				// All knives are terrorist knives.
				if (const auto active_conf = g_config.get_by_definition_index(IsKnife(definition_index) ? WEAPON_KNIFE : definition_index))
					apply_config_on_attributable_item(weapon, active_conf, player_info.m_nXuidLow);
				else
					erase_override_if_exists_by_index(definition_index);
			}
		}
		#pragma endregion

		const auto view_model_handle = pLocal->GetViewModel();

		if (view_model_handle == INVALID_EHANDLE_INDEX)
			return;

		const auto view_model = static_cast<CBaseViewModel*>(ENTITYLIST->GetClientEntityFromHandle(view_model_handle));

		if (!view_model)
			return;

		const auto view_model_weapon_handle = view_model->GetWeapon();

		if (view_model_weapon_handle == INVALID_EHANDLE_INDEX)
			return;

		const auto view_model_weapon = static_cast<CBaseAttributableItem*>(ENTITYLIST->GetClientEntityFromHandle(view_model_weapon_handle));

		if (!view_model_weapon)
			return;

		if (k_weapon_info.count(view_model_weapon->GetItemDefinitionIndex()))
		{
			const auto override_model = k_weapon_info.at(view_model_weapon->GetItemDefinitionIndex()).model;
			auto override_model_index = MODELINFO->GetModelIndex(override_model);
			view_model->GetModelIndex() = override_model_index;

			auto world_model_handle = view_model_weapon->GetWeaponWorldModel();

			if (world_model_handle == INVALID_EHANDLE_INDEX)
				return;

			const auto world_model = (C_BaseWeaponWorldModel*)(ENTITYLIST->GetClientEntityFromHandle(world_model_handle));

			if (!world_model)
				return;

			world_model->SetModelIndex(override_model_index + 1);
		}
	}

};

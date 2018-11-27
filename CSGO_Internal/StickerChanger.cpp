#include "StickerChanger.h"
#include "SkinConfiguration.h"
#include "Utils.hpp"

enum class EStickerAttributeType
{
	Index,
	Wear,
	Scale,
	Rotation
};

static uint16_t s_econ_item_interface_wrapper_offset = 0;

namespace hooks
{
	static void* oGetStickerAttributeBySlotIndexFloat;

	static float __fastcall GetStickerAttributeBySlotIndexFloat(void* thisptr, void*, int slot, EStickerAttributeType attribute, float unknown)
	{
		auto item = reinterpret_cast<CBaseAttributableItem*>(uintptr_t(thisptr) - s_econ_item_interface_wrapper_offset);

		auto defindex = item->GetItemDefinitionIndex();

		auto config = g_config.get_by_definition_index(defindex);

		if (config)
		{
			switch (attribute)
			{
			case EStickerAttributeType::Wear:
				return config->stickers[slot].wear;
			case EStickerAttributeType::Scale:
				return config->stickers[slot].scale;
			case EStickerAttributeType::Rotation:
				return config->stickers[slot].rotation;
			default:
				break;
			}
		}

		return reinterpret_cast<decltype(GetStickerAttributeBySlotIndexFloat)*>(oGetStickerAttributeBySlotIndexFloat)(thisptr, nullptr, slot, attribute, unknown);
	}

	static void* oGetStickerAttributeBySlotIndexInt;

	static unsigned int __fastcall GetStickerAttributeBySlotIndexInt(void* thisptr, void*, int slot, EStickerAttributeType attribute, unsigned unknown)
	{
		auto item = reinterpret_cast<CBaseAttributableItem*>(uintptr_t(thisptr) - s_econ_item_interface_wrapper_offset);

		if (attribute == EStickerAttributeType::Index)
		{
			auto defindex = item->GetItemDefinitionIndex();

			auto config = g_config.get_by_definition_index(defindex);

			if (config)
				return config->stickers[slot].kit_index;
		}

		return reinterpret_cast<decltype(GetStickerAttributeBySlotIndexInt)*>(oGetStickerAttributeBySlotIndexInt)(thisptr, nullptr, slot, attribute, unknown);
	}
}

void apply_sticker_changer(CBaseAttributableItem* item)
{
	if (!s_econ_item_interface_wrapper_offset)
		s_econ_item_interface_wrapper_offset = 0x2DBC; //see below for this offset
	
	// GET_NETVAR(XorStr("DT_EconEntity"), XorStr("m_AttributeManager"), XorStr("m_Item")) + 0xC;
	/*
	 DT_EconEntity
		| ___m_AttributeManager : 0x00002D70
			| ___m_Item : 0x00000040

	+ 0xC = 0X2DBC
	*/

	void**& vmt = *reinterpret_cast<void***>(uintptr_t(item) + s_econ_item_interface_wrapper_offset);

	static void** hooked_vmt = nullptr;

	if (!hooked_vmt)
	{
		size_t size = 0;

		while (Utils::IsCodePtr(vmt[size]))
			++size;

		hooked_vmt = new void*[size];
		memcpy(hooked_vmt, vmt, size * sizeof(void*));

		hooks::oGetStickerAttributeBySlotIndexFloat = hooked_vmt[4];
		hooked_vmt[4] = reinterpret_cast<void*>(&hooks::GetStickerAttributeBySlotIndexFloat);

		hooks::oGetStickerAttributeBySlotIndexInt = hooked_vmt[5];
		hooked_vmt[5] = reinterpret_cast<void*>(&hooks::GetStickerAttributeBySlotIndexInt);
	}

	vmt = hooked_vmt;
}
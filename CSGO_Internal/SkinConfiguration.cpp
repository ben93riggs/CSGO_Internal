#include "SkinConfiguration.h"
#include "SourceEngine/SDK.hpp"
#include "json.hpp"
#include <fstream>
#include "XorStr.hpp"
#include "Globals.h"

SkinConfig g_config;

using json = nlohmann::json;

void to_json(json& j, const StickerSetting& sticker)
{
	j = json
	{
		{ XorStr("kit"), sticker.kit_index },
		{ XorStr("wear"), sticker.wear },
		{ XorStr("scale"), sticker.scale },
		{ XorStr("rotation"), sticker.rotation }
	};
}

void from_json(const json& j, StickerSetting& sticker)
{
	sticker.kit_index = j.at(XorStr("kit")).get<int>();
	sticker.wear = j.at(XorStr("wear")).get<float>();
	sticker.scale = j.at(XorStr("scale")).get<float>();
	sticker.rotation = j.at(XorStr("rotation")).get<float>();
	sticker.update_ids();
}

void to_json(json& j, const EconomyItem_t& item)
{
	j = json
	{
		{ XorStr("name"), item.name },
		{ XorStr("enabled"), item.enabled },
		{ XorStr("definition_index"), item.definition_index },
		{ XorStr("entity_quality_index"), item.entity_quality_index },
		{ XorStr("paint_kit_index"), item.paint_kit_index },
		{ XorStr("definition_override_index"), item.definition_override_index },
		{ XorStr("seed"), item.seed },
		{ XorStr("stat_trak"), item.stat_trak },
		{ XorStr("wear"), item.wear },
		{ XorStr("custom_name"), item.custom_name },
		{ XorStr("stickers"), item.stickers },
	};
}

void from_json(const json& j, EconomyItem_t& item)
{
	strcpy_s(item.name, j.at(XorStr("name")).get<std::string>().c_str());
	item.enabled = j.at(XorStr("enabled")).get<bool>();
	item.definition_index = j.at(XorStr("definition_index")).get<int>();
	item.entity_quality_index = j.at(XorStr("entity_quality_index")).get<int>();
	item.paint_kit_index = j.at(XorStr("paint_kit_index")).get<int>();
	item.definition_override_index = j.at(XorStr("definition_override_index")).get<int>();
	item.seed = j.at(XorStr("seed")).get<int>();
	item.stat_trak = j.at(XorStr("stat_trak")).get<int>();
	item.wear = j.at(XorStr("wear")).get<float>();
	strcpy_s(item.custom_name, j.at(XorStr("custom_name")).get<std::string>().c_str());
	auto stickers = j.at(XorStr("stickers")).get<std::vector<StickerSetting>>();

	const auto size = std::min(stickers.size(), item.stickers.size());

	for (size_t i = 0; i < size; ++i)
		item.stickers.at(i) = stickers.at(i);

	item.update_ids();
}

void SkinConfig::save()
{
	std::ofstream(std::string(getenv(XorStr("APPDATA"))) + XorStr("\\CSGO_Internal\\") + XorStr("skinconfig.json")) << json(m_items);
}

bool fileexists(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

void SkinConfig::load()
{
	std::string file = std::string(getenv(XorStr("APPDATA"))) + XorStr("\\CSGO_Internal\\") + XorStr("skinconfig.json");

	if (fileexists(file.c_str()))
	{
		m_items = json::parse(std::ifstream(file)).get<std::vector<EconomyItem_t>>();
		globals::get().full_update = true;
	}
}

EconomyItem_t* SkinConfig::get_by_definition_index(int definition_index)
{
	for (auto& x : m_items)
		if (x.enabled && x.definition_index == definition_index)
			return &x;

	return nullptr;
}

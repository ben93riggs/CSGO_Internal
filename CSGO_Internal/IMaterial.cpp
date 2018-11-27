#include "SourceEngine/IMaterial.h"
#include "Utils.hpp"

void InitMat::InitKeyValues(KeyValues* pKeyValues, const char* name) const
{
	using InitKeyValuesFn = void(__thiscall*)(void*, const char*);
	static InitKeyValuesFn InitKeyValuesEx = (InitKeyValuesFn)Utils::FindSignature(Utils::get_client_dll(), XorStr("55 8B EC 51 33 C0 C7 45"));

	if (!InitKeyValuesEx)
		return;

	InitKeyValuesEx(pKeyValues, name);
}

void InitMat::LoadFromBuffer(KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc) const
{
	using LoadFromBufferFn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*);
	static LoadFromBufferFn LoadFromBufferEx = (LoadFromBufferFn)Utils::FindSignature(Utils::get_client_dll(), XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04"));

	if (!LoadFromBufferEx)
		return;

	LoadFromBufferEx(pKeyValues, resourceName, pBuffer, pFileSystem, pPathID, pfnEvaluateSymbolProc);
}

IMaterial* IMaterialSystem::FindMaterial(char const* pMaterialName, const char* pTextureGroupName, bool complain, const char* pComplainPrefix)
{
	typedef IMaterial*(__thiscall* oFindMaterial)(PVOID, char const*, char const*, bool, char const*);
	return CallVFunction<oFindMaterial>(this, 84)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
}

MaterialHandle_t IMaterialSystem::FirstMaterial()
{
	typedef MaterialHandle_t (__thiscall* FirstMaterialFn)(void*);
	return CallVFunction<FirstMaterialFn>(this, 86)(this);
}

MaterialHandle_t IMaterialSystem::NextMaterial(MaterialHandle_t h)
{
	typedef MaterialHandle_t (__thiscall* NextMaterialFn)(void*, MaterialHandle_t);
	return CallVFunction<NextMaterialFn>(this, 87)(this, h);
}

MaterialHandle_t IMaterialSystem::InvalidMaterial()
{
	typedef MaterialHandle_t (__thiscall* InvalidMaterialFn)(void*);
	return CallVFunction<InvalidMaterialFn>(this, 88)(this);
}

IMaterial* IMaterialSystem::GetMaterial(MaterialHandle_t h)
{
	if (!h)
		return nullptr;

	typedef IMaterial*(__thiscall* GetMaterialFn)(void*, MaterialHandle_t);
	return CallVFunction<GetMaterialFn>(this, 89)(this, h);
}

IMaterial* IMaterialSystem::CreateMaterial(bool flat, bool ignorez, bool wireframed)
{
	std::string type = (flat) ? XorStr("UnlitGeneric") : XorStr("VertexLitGeneric");

	std::string matdata = XorStr("\"") + type + XorStr("\"\n{\n\t\"$basetexture\" \"vgui/white_additive\"\n\t\"$envmap\"  \"\"\n\t\"$model\" \"1\"\n\t\"$flat\" \"1\"\n\t\"$nocull\"  \"0\"\n\t\"$selfillum\" \"1\"\n\t\"$halflambert\" \"1\"\n\t\"$nofog\"  \"0\"\n\t\"$znearer\" \"0\"\n\t\"$wireframe\" \"") + std::to_string(wireframed) + XorStr("\"\n\t\"$ignorez\" \"") + std::to_string(ignorez) + XorStr("\"\n}\n");

	std::string matname = XorStr("custom_") + std::to_string(InitMat::get().created);
	InitMat::get().created++;

	KeyValues* pKeyValues = new KeyValues(matname.c_str());
	InitMat::get().InitKeyValues(pKeyValues, type.c_str());
	InitMat::get().LoadFromBuffer(pKeyValues, matname.c_str(), matdata.c_str());

	typedef IMaterial*(__thiscall* OriginalFn)(void*, const char* pMaterialName, KeyValues* pVMTKeyValues);
	IMaterial* createdMaterial = CallVFunction<OriginalFn>(this, 83)(this, matname.c_str(), pKeyValues);

	createdMaterial->IncrementReferenceCount();

	return createdMaterial;
}

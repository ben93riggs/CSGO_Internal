#pragma once
#include "Singleton.h"
#include "SourceEngine/IMaterial.h"

typedef void(__thiscall* DrawModelExecute_t)(IVModelRender*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);

class Chams : public singleton<Chams>
{
	enum
	{
		CHAMS_OFF,
		CHAMS_NORMAL,
		CHAMS_WIREFRAME,
		CHAMS_REMOVE
	};

public:
	IMaterial* mat = nullptr;
	DrawModelExecute_t original_func;

	static void force_matrial(IMaterial* mat = nullptr, Color color = Color(255, 255, 255));

	void player_chams(C_CSPlayer* pLocal, IVModelRender* ecx, void* context, void* state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld, bool& shouldntdraw) const;

	void weapons_normal();
	void weapons_wireframe();
	void weapons_remove(std::string modelName);
	void weapons_chams(C_CSPlayer* pLocal, std::string modelName);

	void arms_normal();
	void arms_wireframe();
	void arms_remove(std::string modelName);
	void arms_chams(std::string modelName);

	void gloves_normal();
	void gloves_wireframe();
	void gloves_remove(std::string modelName);
	void gloves_chams(std::string modelName);

	void draw_model_execute(DrawModelExecute_t original_func, IVModelRender* ecx, void* context, void* state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld, bool& shouldntdraw);
	static void scene_end();
};

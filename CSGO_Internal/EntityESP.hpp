#pragma once
#include "Singleton.h"
#include "SourceEngine/Color.hpp"
#include "Vars.h"

class C_CSPlayer;

class entity_esp : public singleton<entity_esp>
{
	static void draw_head(C_CSPlayer*, Color& color, matrix3x4_t* matrix);
	static void draw_backtrack_skeletons(C_CSPlayer*, C_CSPlayer*, Color& color);
public:
	static void draw_bomb(C_CSPlayer*);
	static void draw_bomb_damage(C_CSPlayer*, int);
	static void draw_item_esp(C_CSPlayer*);
	void do_player_esp(C_CSPlayer*, C_CSPlayer*) const;
	static void draw_bounds(C_CSPlayer*, Color& color, kBoxSettings box_mode);
	static void draw_hitboxes(C_CSPlayer*, Color& color, matrix3x4_t* matrix);
};
#pragma once
#include "../Resolver.h"
#include "../WorldChams.h"
#include "../NoVisualRecoil.h"
#include "../ThirdPersonAA.h"
#include "../SkinChanger.h"
#include "../PVSFix.h"
#include "../NoSmoke.h"
#include "../NoFlash.h"
#include "../NoScope.h"
#include "../Radar.h"
#include "../ASUSWalls.h"
#include "../ClanTag.h"
#include "../WeaponConfigs.h"
#include "../LagCompensation.h"
#include "../Accuracy.h"

typedef void(__thiscall* frame_stage_notify_t)(void*, ClientFrameStage_t);
frame_stage_notify_t o_frame_stage_notify;

inline void __fastcall hooks::frame_stage_notify(void* ecx, void* edx, ClientFrameStage_t stage)
{
	static auto old_net_channel = CLIENTSTATE->net_channel;
	if (!p_net_chan || old_net_channel != CLIENTSTATE->net_channel)
	{
		if (CLIENTSTATE->net_channel)
		{
			p_net_chan = std::make_shared<VFTableHook>(reinterpret_cast<PPDWORD>(CLIENTSTATE->net_channel), true);
			o_send_datagram = p_net_chan->Hook(46, reinterpret_cast<send_datagram_t>(send_datagram));
		}

		old_net_channel = CLIENTSTATE->net_channel;
	}

	if (!MenuVars::get().g_bFrameStageNotify)
		return o_frame_stage_notify(ecx, stage);

	static auto initialized = false;
	if (!initialized)
	{
		InitializeKits();
		g_config.load();
		initialized = true;
	}

	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
	{
		LagCompensation::get().ClearIncomingSequences();
		return o_frame_stage_notify(ecx, stage);
	}

	if (globals::get().full_update)
	{
		CLIENTSTATE->force_full_update();
		globals::get().full_update = false;
		return;
	}

	ASUSWalls::get().FrameStageNotify(stage);
	WorldChams::get().FrameStageNotify(stage);
	PVSFix::FrameStageNotify(stage);
	ThirdPersonAA::get().FrameStageNotify_pre(stage);
	NoFlash::FrameStageNotify(stage);
	NoScope::FrameStageNotify(stage);
	NoSmoke::get().FrameStageNotify(stage);
	NoVisualRecoil::get().FrameStageNotify_pre(stage);

	WeaponConfigs::FrameStageNotify(stage);

	ClanTag::get().FrameStageNotify(stage);
	SkinChanger::get().FrameStageNotify(stage);
	Radar::FrameStageNotify(stage);

	//DisableInterpolation::get().FrameStageNotify(stage);
	Accuracy::get().FrameStageNotify(stage);

	o_frame_stage_notify(ecx, stage);
	
	//FRAME_RENDER_START
	NoVisualRecoil::get().FrameStageNotify_post(stage);
	ThirdPersonAA::get().FrameStageNotify_post(stage);
}
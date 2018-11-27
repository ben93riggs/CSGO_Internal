#pragma once
#include "../MoveFix.h"
#include "../PredictionSystem.h"
#include "../Triggerbot.h"
#include "../FakeLag.h"
#include "../RCS.h"
#include "../Spammers.h"
#include "../AutoPistol.h"
#include "../BHop.h"
#include "../Globals.h"
#include "../GrenadePrediction.h"
#include "../Aimbot.h"
#include "../Knifebot.h"
#include "../RankRevealer.h"
#include "../AntiAim.h" 
#include "../Hooks.hpp"
#include "../TickFix.h"
#include "../NameStealer.h"
#include "../LagCompensation.h"
#include "../ClosestToCrosshair.h"

std::shared_ptr<hooks::VFTableHook>	p_net_chan;
typedef int(__thiscall* send_datagram_t)(INetChannel*, void*);
send_datagram_t o_send_datagram;

inline int __fastcall hooks::send_datagram(INetChannel* ecx, void* edx, void* datagram)
{
	if (!MenuVars::get().g_bBackTrackExploit)
		return o_send_datagram(ecx, datagram);

	if (datagram)
		return o_send_datagram(ecx, datagram);

	const auto local = C_CSPlayer::GetLocalPlayer();

	if (!local)
		return o_send_datagram(ecx, datagram);

	const auto nci = ENGINE->GetNetChannelInfo();

	if (!nci)
		return o_send_datagram(ecx, datagram);

	const auto current_ping = nci->GetLatency(FLOW_OUTGOING);
	const auto goal_ping = .2f - current_ping;

	if (MenuVars::get().g_bBackTrackExploit)
	{
		const auto instate = ecx->m_nInReliableState;
		const auto insequencenr = ecx->m_nInSequenceNr;

		if (current_ping <= goal_ping)
			LagCompensation::get().AddLatencyToNetchan(ecx, goal_ping);
		else 
			return o_send_datagram(ecx, datagram);

		const auto ret = o_send_datagram(ecx, datagram);

		ecx->m_nInReliableState = instate;
		ecx->m_nInSequenceNr = insequencenr;

		return ret;
	}

	return o_send_datagram(ecx, datagram);
}

typedef bool (__thiscall* create_move_t)(IClientMode*, float, CUserCmd*);
create_move_t o_create_move;

inline bool __fastcall hooks::create_move(IClientMode* ecx, void* edx, float sample_input_frametime, CUserCmd* user_cmd)
{
	if (!MenuVars::get().g_bCreateMove)
		return o_create_move(ecx, sample_input_frametime, user_cmd);

	if (!user_cmd->command_number)
		return o_create_move(ecx, sample_input_frametime, user_cmd);

	auto& send_packet = *reinterpret_cast<bool*>(*reinterpret_cast<uintptr_t*>(static_cast<uintptr_t*>(_AddressOfReturnAddress()) - 1) - 0x1C); // - 1 used to be - 0x4 but pointer math is goof'd

	auto local = C_CSPlayer::GetLocalPlayer();
	if (!local) return o_create_move(ecx, sample_input_frametime, user_cmd);

	NameStealer::get().Do(local);

	//set/update our global variables
	g::cmd = user_cmd;

	TickFix::GetTickbase(g::cmd, local); // Update tickbase correction.

	const auto org_angle = g::cmd->viewangles;
	g::velocity_unpredicted = local->GetVelocity();
	auto ret = true;

	LagCompensation::get().UpdateIncomingSequences();

	spammers::create_move();
	RankRevealer::get().CreateMove();

	if (!local->IsAlive())
		return o_create_move(ecx, sample_input_frametime, user_cmd);

	//FakeWalk::get().Fakewalk(G::UserCmd, bSendPacket);
	anti_aim::get().update_lby_animations();
	CCSGrenadeHint::get().Tick(g::cmd->buttons);
	BunnyHop::get().CreateMove();

	const auto wishangle = g::cmd->viewangles;
	g::cmd->viewangles = org_angle;
	
	PredictionSystem::get().Start(g::cmd);

	anti_aim::get().fakewalk(g::cmd, send_packet);
	closest_to_crosshair::run(g::cmd);
	Knifebot::get().CreateMove();
	Aimbot::CreateMove(ret, send_packet);
	AutoPistol::CreateMove();
	RCS::get().CreateMove(ret);
	Triggerbot::get().CreateMove();
	g::fakelagging = fake_lag::create_move(send_packet);
	anti_aim::get().create_move(ret, send_packet);
	MoveFix::get().Fix(g::cmd, wishangle);
	BunnyHop::get().AirCrouch(local);

	PredictionSystem::get().Finish();

	Math::ClampMovement(g::cmd);
	Math::Clamp(g::cmd->viewangles);

	if (send_packet)
	{
		g::previous_position = local->GetAbsOrigin();
		g::anti_aim_fake_angles = g::cmd->viewangles; //set AA angles
	}
	else
	{
		g::anti_aim_angles = g::cmd->viewangles; //set AA angles
	} 

	if (!AAVars::get().aa[g::i_current_aa].g_bShouldFake && (MiscVars::get().g_nFakeLag == FLAG_OFF || MiscVars::get().g_nFakeLag == FLAG_ADAPTIVE && local->GetFlags() & FL_ONGROUND))
	{
		g::anti_aim_angles = g::cmd->viewangles;
		g::anti_aim_fake_angles = g::cmd->viewangles;
	}
	
	g::anti_aim_angles.x = g::cmd->viewangles.x;
	g::anti_aim_fake_angles.x = g::cmd->viewangles.x;

	if (CLIENTSTATE->chokedcommands > 20 && !send_packet && !(MiscVars::get().g_bFakeLagOnKey && INPUTSYSTEM->IsButtonDown(static_cast<ButtonCode_t>(MiscVars::get().g_kLagKey)) && CLIENTSTATE->chokedcommands < 100))
		send_packet = true;

	return ret;
}
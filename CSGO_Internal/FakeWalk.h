#pragma once
#include "Singleton.h"
#include "SourceEngine/Vector.hpp"
#include "BHop.h"

class CUserCmd;

class FakeWalk : public singleton<FakeWalk>
{
	static void Accelerate(C_CSPlayer* player, Vector& wishdir, float wishspeed, float accel, Vector& outVel);
	void WalkMove(C_CSPlayer* player, Vector& outVel,float,float) const;
	static void Friction(C_CSPlayer* pLocal, Vector& outVel);

	int last_choked_ticks = 0;

public:
	void Fakewalk(CUserCmd* cmd, bool& bSendPacket) const;
};

//namespace FakeWalk
//{
//	inline int GetCurTick(C_CSPlayer* pLocal)
//	{
//		if (!G::UserCmd)
//			return 0;
//
//		if (G::UserCmd->hasbeenpredicted)
//			return pLocal->GetTickBase();
//
//		return pLocal->GetTickBase() + 1;
//	}
//
//	inline float GetCurTime(C_CSPlayer* pLocal)
//	{
//		if (!G::UserCmd)
//			return 0;
//
//		if (G::UserCmd->hasbeenpredicted)
//			return TICKS_TO_TIME(pLocal->GetTickBase());
//
//		return TICKS_TO_TIME(pLocal->GetTickBase() + 1);
//	}
//
//	inline void CreateMove2(C_CSPlayer* pLocal, bool& bSendPacket, bool& bRet)
//	{
//		if (INPUTSYSTEM->IsButtonDown(KEY_H))
//		{
//			G::bInFakeWalk = true;
//
//			Vector velocity = G::m_vecVelocityUnpredicted;
//
//			static auto predict_velocity = [](Vector* velocity) 
//			{
//				static auto sv_friction = CVAR->FindVar(XorStr("sv_friction"));
//				static auto sv_stopspeed = CVAR->FindVar(XorStr("sv_stopspeed"));
//
//				float speed = velocity->Length();
//				if (speed >= 0.1f) 
//				{
//					float friction = sv_friction->GetFloat();
//					float stop_speed = std::max<float>(speed, sv_stopspeed->GetFloat());
//					float time = std::max<float>(GLOBALS->interval_per_tick, GLOBALS->frametime);
//					*velocity *= std::max<float>(0.f, speed - friction * stop_speed * time / speed);
//				}
//			};
//
//			static auto quick_stop = []() 
//			{
//				Vector velocity = G::m_vecVelocityUnpredicted;
//				float speed = velocity.Length2D();
//				if (speed > 13.f) 
//				{
//					QAngle direction;
//					Math::VectorAngles2(G::m_vecVelocityUnpredicted, direction);
//
//					direction.y = G::UserCmd->viewangles.y - direction.y;
//
//					Vector new_move;
//					Math::AngleVectors(direction, &new_move);
//					new_move *= -450.f;
//
//					G::backup_cmd.forwardmove = new_move.x;
//					G::backup_cmd.sidemove = new_move.y;
//				}
//				else 
//				{
//					G::backup_cmd.forwardmove = 0.f;
//					G::backup_cmd.sidemove = 0.f;
//				}
//			};
//
//			int ticks_to_stop;
//			for (ticks_to_stop = 0; ticks_to_stop < 15; ++ticks_to_stop) 
//			{
//				if (velocity.Length2D() < 0.1f)
//					break;
//
//				predict_velocity(&velocity);
//			}
//
//			//check how many ticks before LBY update comes in
//			//int ticks_to_update = m_breaker.get_next_update() - 1;
//			int ticks_to_update = (TIME_TO_TICKS(G::next_body_update_) - GetCurTick(pLocal)) - 1;
//			int max_ticks = std::min<int>(13, ticks_to_update);
//			int choked = CLIENTSTATE->net_channel->m_nChokedPackets;
//			int ticks_left = max_ticks - choked;
//
//			if (choked < max_ticks || ticks_to_stop) //if we havent hit max ticks or arent already stopped, choke
//				bSendPacket = false;
//
//			if (ticks_to_stop > ticks_left || !choked || bSendPacket) //if we still need to slow down or we're sending a packet or havent choked any packets yet, stay stopped
//				quick_stop();
//
//		}
//		else
//		{
//			G::bInFakeWalk = false;
//		}
//
//		bRet = false;
//	}
//
//
//	inline void CreateMove(C_CSPlayer* pLocal, bool& bSendPacket, bool& bRet)
//	{
//		if (INPUTSYSTEM->IsButtonDown(KEY_H))
//		{
//			G::bInFakeWalk = true;
//
//			QAngle direction;
//			Math::VectorAngles3(G::m_vecVelocityUnpredicted, direction);
//
//			float speed = G::m_vecVelocityUnpredicted.Length();
//
//			direction.y = G::UserCmd->viewangles.y - direction.y;
//
//			Vector negated_direction = direction.forward() * -speed;
//
//			if (G::m_vecVelocityUnpredicted.Length() < 20.f)
//			{
//				negated_direction.x = 0.f;
//				negated_direction.y = 0.f;
//			}
//
//			int choke = CLIENTSTATE->net_channel->m_nChokedPackets;
//
//			if (choke <= 1 || choke >= 7)
//			{
//				G::UserCmd->forwardmove = negated_direction.x;
//				G::UserCmd->sidemove = negated_direction.y;
//				G::UserCmd->buttons |= IN_WALK;
//			}
//
//			if (G::m_vecVelocityUnpredicted.Length() == 0.f/* && pLocal->GetVelocity().Length() == 0.f*/ && choke >= 7)
//			{
//				bSendPacket = true;
//			}
//			else bSendPacket = false;
//
//		}
//		else
//		{
//			G::bInFakeWalk = false;
//		}
//
//		bRet = false;
//	}
//}
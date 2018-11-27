#pragma once
#include "Singleton.h"
#include "SourceEngine/Vector.hpp"
#include "SourceEngine/CInput.hpp"

class C_CSPlayer;

class Legitbot : public singleton<Legitbot>
{
public:


	static bool AimKeyPressed(C_CSPlayer* pLocal);
	static void DoAutoStop(C_CSPlayer* local);
	static void DoAutoDuck(C_CSPlayer* local);
	static bool GetBestBoneTarget(C_CSPlayer* pLocal, C_CSPlayer* pEntity, Vector& vec_out);
	float GetRealDistanceFOV(float distance, QAngle angle, CUserCmd* cmd) const;
	bool Hitchance(C_CSPlayer * pLocal, QAngle & angle) const;
	static void ApplyErrorToAngle(QAngle* angles, float margin);
	void Smooth(C_CSPlayer* player, QAngle& angle, CUserCmd* cmd) const;
	bool GetBestTarget(C_CSPlayer* pLocal, C_CSPlayer*& target_out) const;
	bool AimAtPlayer(C_CSPlayer* pLocal, C_CSPlayer* target, bool& bRet, bool& bSendPacket) const;
	static bool AimWithBacktrack(C_CSPlayer* pLocal, C_CSPlayer* target, bool& bRet, bool& bSendPacket);
	void CreateMove(bool& bRet, bool& bSendPacket) const;
};
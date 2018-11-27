#pragma once
#include "C_CSPlayer.h"
#include "Singleton.h"

class RageBot : public singleton<RageBot>
{
	enum
	{
		FL_HIGH = (1 << 0),
		FL_LOW = (1 << 1),
		FL_SPECIAL = (1 << 2)
	};

	struct BestPoint
	{
		BestPoint()
		{
			hitbox = 0;
			point = Vector(0, 0, 0);
			index = 0;
			dmg = 0;
			flags = 0;
		}

		void Reset()
		{
			hitbox = 0;
			point = Vector(0, 0, 0);
			index = 0;
			dmg = 0;
			flags = 0;
		}

		int hitbox;
		Vector point;
		int index;
		int dmg;
		int flags;

	};

public:
	struct Hitbox
	{
		Hitbox()
		{
			hitbox = NULL;
		}

		Hitbox(int newHitBox)
		{
			hitbox = newHitBox;
		}

		int hitbox;
		Vector points[9];
		BestPoint bestpoint;
	};

private:
	struct HitboxSetInfo
	{
		bool isCurrent = false;
		matrix3x4_t matrix[MAXSTUDIOBONES];
		mstudiohitboxset_t* set;
	};

	QAngle aimangle;
	int bestindex = -1;
	QAngle LastAngle;
	bool bFinished = false;
	C_CSPlayer* besttarget = nullptr;

	void DoAutoStop(C_CSPlayer* local) const;
	static void DoAutoDuck(C_CSPlayer* local);

	static bool AimKeyPressed();

	static bool IsValidTarget(C_CSPlayer * ent, C_CSPlayer * pLocal);
	static bool InSelectedHitGroup(int hitgroup);
	bool Hitchance(C_CSPlayer * local, QAngle& EnemyAngle) const;
	static bool Aimstep(QAngle& src, QAngle& dst, QAngle& out, int steps);

	struct HitScan : singleton<HitScan>
	{
		std::vector<Hitbox> boxes;
		void add_hitbox(CSGOHitboxID box);
		void clear_hitboxes();
		void fill_hitboxes(bool blacklist_head);
		bool Scan(C_CSPlayer* pEntity, C_CSPlayer* pLocal, Vector& position_out, float& dmg_out); //entry point
		static bool MultiPointHitbox(C_CSPlayer* ent, Hitbox& hitbox_t, C_CSPlayer* pLocal, matrix3x4_t* matrix);
	};

	float FindTarget(C_CSPlayer* pLocal);
	void AutoFire(C_CSPlayer* pLocal, bool& bSendPacket) const;
	bool Aim(C_CSPlayer* pLocal, bool& bRet, bool& bSendPacket, float dmg);

	//entry
public:
	void CreateMove(bool& bRet, bool& bSendPacket);
	bool Hitchance(C_CSPlayer * pLocal, C_CSPlayer * pEnt, QAngle & angle) const;
};

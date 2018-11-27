#include "FakeWalk.h"
#include "Globals.h"


void FakeWalk::Accelerate(C_CSPlayer *player, Vector &wishdir, float wishspeed, float accel, Vector &outVel)
{
	// See if we are changing direction a bit
	float currentspeed = outVel.Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	float addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
		return;

	// Determine amount of accleration.
	float accelspeed = accel * GLOBALS->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	for (int i = 0; i < 3; i++)
		outVel[i] += accelspeed * wishdir[i];
}

void FakeWalk::WalkMove(C_CSPlayer *player, Vector &outVel, float fmove, float smove) const
{
	Vector forward, right, up, wishvel;

	Math::AngleVectors(player->GetEyeAngles(), forward, right, up);  // Determine movement angles

	if (forward[2] != 0)
	{
		forward[2] = 0;
		Math::NormalizeVector(forward);
	}

	if (right[2] != 0)
	{
		right[2] = 0;
		Math::NormalizeVector(right);
	}

	for (int i = 0; i < 2; i++)	// Determine x and y parts of velocity
		wishvel[i] = forward[i] * fmove + right[i] * smove;

	wishvel[2] = 0;	// Zero out z part of velocity

	Vector wishdir = wishvel; // Determine maginitude of speed of move
	float wishspeed = wishdir.Normalize2();

	// Clamp to server defined max speed
	Interface->MoveHelper()->SetHost(player);
	if ((wishspeed != 0.0f) && (wishspeed > Interface->MoveHelper()->m_flMaxSpeed))
	{
		VectorMultiply(wishvel, player->m_flMaxspeed() / wishspeed, wishvel);
		wishspeed = player->m_flMaxspeed();
	}
	Interface->MoveHelper()->SetHost(nullptr);
	// Set pmove velocity
	outVel[2] = 0;
	Accelerate(player, wishdir, wishspeed, CVAR->FindVar(XorStr("sv_accelerate"))->GetFloat(), outVel);
	outVel[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(outVel, player->m_vecBaseVelocity(), outVel);

	float spd = outVel.Length();

	if (spd < 1.0f)
	{
		outVel.Init();
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
		return;
	}

	// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
}

void FakeWalk::Friction(C_CSPlayer* pLocal, Vector &outVel)
{
	float speed = outVel.Length();

	if (speed < 0.1f)
		return;

	float drop = 0;

	// apply ground friction
	if (pLocal->GetFlags() & FL_ONGROUND)
	{
		float friction = CVAR->FindVar(XorStr("sv_friction"))->GetFloat() * pLocal->m_surfaceFriction();

		// Bleed off some speed, but if we have less than the bleed
		// threshold, bleed the threshold amount.

		float control = (speed < CVAR->FindVar(XorStr("sv_stopspeed"))->GetFloat()) ? CVAR->FindVar(XorStr("sv_stopspeed"))->GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * GLOBALS->frametime;
	}

	float newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;
		// Adjust velocity according to proportion.
		VectorMultiply(outVel, newspeed, outVel);
	}
}

void FakeWalk::Fakewalk(CUserCmd* cmd, bool& bSendPacket) const
{
	g::b_in_fake_walk = false;

	if (!MenuVars::get().g_bFakewalk || !INPUTSYSTEM->IsButtonDown(MenuVars::get().g_kFakeWalkKey))
		return;

	C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal)
		return;

	if (!pLocal->IsAlive())
		return;

	if (!(pLocal->GetFlags() & FL_ONGROUND))
		return;

	Vector velocity = g::velocity_unpredicted;
	int choked_ticks = CLIENTSTATE->chokedcommands;
	cmd->buttons &= ~IN_WALK;

	if(choked_ticks == 0) 
	{
		if (velocity.Length() > 0.1) 
		{
			float_t speed = velocity.Length();

			QAngle direction;
			Math::VectorAngles(velocity, direction);

			direction.y = cmd->viewangles.y - direction.y;

			Vector forward;
			Math::AngleVectors(direction, &forward);
			Vector negated_direction = forward * -speed;

			cmd->forwardmove = negated_direction.x;
			cmd->sidemove = negated_direction.y;
		}
		else 
		{
			cmd->forwardmove = 0;
			cmd->sidemove = 0;
		}
	}
	else {
		int Iterations = choked_ticks;
		while (true) 
		{
			if (velocity.Length() < 0.1) 
			{
				//std::cout << "Ticks till stop: " << Iterations << std::endl;
				break;
			}
			Friction(pLocal, velocity);
			//WalkMove(pLocal, velocity);
			Iterations++;
		}

		//std::cout << Iterations << std::endl;

		if (Iterations >= 13) {
			float_t speed = velocity.Length();

			QAngle direction;
			Math::VectorAngles(velocity, direction);

			direction.y = cmd->viewangles.y - direction.y;

			Vector forward;
			Math::AngleVectors(direction, &forward);
			Vector negated_direction = forward * -speed;

			cmd->forwardmove = negated_direction.x;
			cmd->sidemove = negated_direction.y;
		}
	}

	if (choked_ticks != 13)
		bSendPacket = false;

	g::b_in_fake_walk = true;
}

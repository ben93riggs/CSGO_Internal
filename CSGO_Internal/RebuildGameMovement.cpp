#include "RebuildGameMovement.h"
#include "Globals.h"

void RebuildGameMovement::SetAbsOrigin(C_CSPlayer* player, const Vector &vec)
{
	player->SetAbsOrigin(vec);
}

int RebuildGameMovement::ClipVelocity(Vector &in, Vector &normal, Vector &out, float overbounce)
{
	float	backoff;
	float	change;
	float angle;
	int		i, blocked;

	angle = normal[2];

	blocked = 0x00;         // Assume unblocked.
	if (angle > 0)			// If the plane that is blocking us has a positive z component, then assume it's a floor.
		blocked |= 0x01;	// 
	if (!angle)				// If the plane has no Z, it is vertical (wall/step)
		blocked |= 0x02;	// 

							// Determine how far along plane to slide based on incoming direction.
	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i<3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
	}

	// iterate once to make sure we aren't still moving through the plane
	float adjust = out.Dot(normal);
	if (adjust < 0.0f)
	{
		out -= (normal * adjust);
		//		Msg( "Adjustment = %lf\n", adjust );
	}

	// Return blocking flags.
	return blocked;
}

int RebuildGameMovement::TryPlayerMove(C_CSPlayer* player, Vector *pFirstDest, trace_t *pFirstTrace)
{
	Vector  planes[5];
	numbumps[player->EntIndex()] = 4;           // Bump up to four times

	blocked[player->EntIndex()] = 0;           // Assume not blocked
	numplanes[player->EntIndex()] = 0;           //  and not sliding along any planes

	original_velocity[player->EntIndex()] = player->GetVelocity(); // Store original velocity
	primal_velocity[player->EntIndex()] = player->GetVelocity();

	allFraction[player->EntIndex()] = 0;
	time_left[player->EntIndex()] = GLOBALS->frametime;   // Total time for this movement operation.

	new_velocity[player->EntIndex()].Zero();

	for (bumpcount[player->EntIndex()] = 0; bumpcount[player->EntIndex()] < numbumps[player->EntIndex()]; bumpcount[player->EntIndex()]++)
	{
		if (player->GetVelocity().Length() == 0.0)
			break;

		// Assume we can move all the way from the current origin to the
		//  end point.
		VectorMA(player->GetAbsOrigin(), time_left[player->EntIndex()], player->GetVelocity(), end[player->EntIndex()]);

		// See if we can make it from origin to end point.
		if (true)
		{
			// If their velocity Z is 0, then we can avoid an extra trace here during WalkMove.
			if (pFirstDest && end[player->EntIndex()] == *pFirstDest)
				pm[player->EntIndex()] = *pFirstTrace;
			else
			{
				TracePlayerBBox(player->GetAbsOrigin(), end[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);
			}
		}
		else
		{
			TracePlayerBBox(player->GetAbsOrigin(), end[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);
		}

		allFraction[player->EntIndex()] += pm[player->EntIndex()].fraction;

		// If we started in a solid object, or we were in solid space
		//  the whole way, zero out our velocity and return that we
		//  are blocked by floor and wall.
		if (pm[player->EntIndex()].allsolid)
		{
			// C_CSPlayer is trapped in another solid
			player->GetVelocity() = vec3_origin[player->EntIndex()];
			return 4;
		}

		// If we moved some portion of the total distance, then
		//  copy the end position into the pmove.origin and 
		//  zero the plane counter.
		if (pm[player->EntIndex()].fraction > 0)
		{
			if (numbumps[player->EntIndex()] > 0 && pm[player->EntIndex()].fraction == 1)
			{
				// There's a precision issue with terrain tracing that can cause a swept box to successfully trace
				// when the end position is stuck in the triangle.  Re-run the test with an uswept box to catch that
				// case until the bug is fixed.
				// If we detect getting stuck, don't allow the movement
				trace_t stuck;
				TracePlayerBBox(pm[player->EntIndex()].endpos, pm[player->EntIndex()].endpos, MASK_PLAYERSOLID, 8, stuck, player);
				if (stuck.startsolid || stuck.fraction != 1.0f)
				{
					//Msg( "Player will become stuck!!!\n" );
					player->GetVelocity() = vec3_origin[player->EntIndex()];
					break;
				}
			}

			// actually covered some distance
			SetAbsOrigin(player, pm[player->EntIndex()].endpos);
			original_velocity[player->EntIndex()] = player->GetVelocity();
			numplanes[player->EntIndex()] = 0;
		}

		// If we covered the entire distance, we are done
		//  and can return.
		if (pm[player->EntIndex()].fraction == 1)
		{
			break;		// moved the entire distance
		}

		// If the plane we hit has a high z component in the normal, then
		//  it's probably a floor
		if (pm[player->EntIndex()].plane.normal[2] > 0.7)
		{
			blocked[player->EntIndex()] |= 1;		// floor
		}
		// If the plane has a zero z component in the normal, then it's a 
		//  step or wall
		if (!pm[player->EntIndex()].plane.normal[2])
		{
			blocked[player->EntIndex()] |= 2;		// step / wall
		}

		// Reduce amount of m_flFrameTime left by total time left * fraction
		//  that we covered.
		time_left[player->EntIndex()] -= time_left[player->EntIndex()] * pm[player->EntIndex()].fraction;

		// Did we run out of planes to clip against?
		if (numplanes[player->EntIndex()] >= 5)
		{
			// this shouldn't really happen
			//  Stop our movement if so.
			player->GetVelocity() = vec3_origin[player->EntIndex()];
			//Con_DPrintf("Too many planes 4\n");

			break;
		}

		// Set up next clipping plane
		planes[numplanes[player->EntIndex()]] = pm[player->EntIndex()].plane.normal;
		numplanes[player->EntIndex()]++;

		// modify original_velocity so it parallels all of the clip planes
		//

		// reflect player velocity 
		// Only give this a try for first impact plane because you can get yourself stuck in an acute corner by jumping in place
		//  and pressing forward and nobody was really using this bounce/reflection feature anyway...
		if (numplanes[player->EntIndex()] == 1 &&
			player->GetFlags() & FL_ONGROUND)
		{
			for (i[player->EntIndex()] = 0; i[player->EntIndex()] < numplanes[player->EntIndex()]; i[player->EntIndex()]++)
			{
				if (planes[i[player->EntIndex()]][2] > 0.7)
				{
					// floor or slope
					ClipVelocity(original_velocity[player->EntIndex()], planes[i[player->EntIndex()]], new_velocity[player->EntIndex()], 1);
					original_velocity[player->EntIndex()] = new_velocity[player->EntIndex()];
				}
				else
				{
					ClipVelocity(original_velocity[player->EntIndex()], planes[i[player->EntIndex()]], new_velocity[player->EntIndex()], 1.0 + CVAR->FindVar(XorStr("sv_bounce"))->GetFloat() * (1 - player->m_surfaceFriction()));
				}
			}

			player->GetVelocity() = new_velocity[player->EntIndex()];
			original_velocity[player->EntIndex()] = new_velocity[player->EntIndex()];
		}
		else
		{
			for (i[player->EntIndex()] = 0; i[player->EntIndex()] < numplanes[player->EntIndex()]; i[player->EntIndex()]++)
			{


				for (j[player->EntIndex()] = 0; j[player->EntIndex()]<numplanes[player->EntIndex()]; j[player->EntIndex()]++)
					if (j[player->EntIndex()] != i[player->EntIndex()])
					{
						// Are we now moving against this plane?
						if (player->GetVelocity().Dot(planes[j[player->EntIndex()]]) < 0)
							break;	// not ok
					}
				if (j[player->EntIndex()] == numplanes[player->EntIndex()])  // Didn't have to clip, so we're ok
					break;
			}

			// Did we go all the way through plane set
			if (i[player->EntIndex()] != numplanes[player->EntIndex()])
			{	// go along this plane
				// pmove.velocity is set in clipping call, no need to set again.
				;
			}
			else
			{	// go along the crease
				if (numplanes[player->EntIndex()] != 2)
				{
					player->GetVelocity() = vec3_origin[player->EntIndex()];
					break;
				}

				dir[player->EntIndex()] = planes[0].Cross(planes[1]);
				dir[player->EntIndex()].NormalizeInPlace();
				d[player->EntIndex()] = dir[player->EntIndex()].Dot(player->GetVelocity());
				VectorMultiply(dir[player->EntIndex()], d[player->EntIndex()], player->GetVelocity());
			}

			//
			// if original velocity is against the original velocity, stop dead
			// to avoid tiny occilations in sloping corners
			//
			d[player->EntIndex()] = player->GetVelocity().Dot(primal_velocity[player->EntIndex()]);
			if (d[player->EntIndex()] <= 0)
			{
				//Con_DPrintf("Back\n");
				player->GetVelocity() = vec3_origin[player->EntIndex()];
				break;
			}
		}
	}

	if (allFraction == 0)
	{
		player->GetVelocity() = vec3_origin[player->EntIndex()];
	}

	// Check if they slammed into a wall
	float fSlamVol = 0.0f;

	float fLateralStoppingAmount = primal_velocity[player->EntIndex()].Length2D() - player->GetVelocity().Length2D();
	if (fLateralStoppingAmount > 580.f * 2.0f)
	{
		fSlamVol = 1.0f;
	}
	else if (fLateralStoppingAmount > 580.f)
	{
		fSlamVol = 0.85f;
	}

	return blocked[player->EntIndex()];
}

void RebuildGameMovement::Accelerate(C_CSPlayer* player, Vector &wishdir, float wishspeed, float accel)
{
	// See if we are changing direction a bit
	currentspeed[player->EntIndex()] = player->GetVelocity().Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	addspeed[player->EntIndex()] = wishspeed - currentspeed[player->EntIndex()];

	// If not going to add any speed, done.
	if (addspeed[player->EntIndex()] <= 0)
		return;

	// Determine amount of accleration.
	accelspeed[player->EntIndex()] = accel * GLOBALS->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed[player->EntIndex()] > addspeed[player->EntIndex()])
		accelspeed[player->EntIndex()] = addspeed[player->EntIndex()];

	// Adjust velocity.
	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<3; i[player->EntIndex()]++)
	{
		player->GetVelocity()[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];
	}
}

void RebuildGameMovement::AirAccelerate(C_CSPlayer* player, Vector &wishdir, float wishspeed, float accel)
{
	wishspd[player->EntIndex()] = wishspeed;

	// Cap speed
	if (wishspd[player->EntIndex()] > 30.f)
		wishspd[player->EntIndex()] = 30.f;

	// Determine veer amount
	currentspeed[player->EntIndex()] = player->GetVelocity().Dot(wishdir);

	// See how much to add
	addspeed[player->EntIndex()] = wishspd[player->EntIndex()] - currentspeed[player->EntIndex()];

	// If not adding any, done.
	if (addspeed <= 0)
		return;

	// Determine acceleration speed after acceleration
	accelspeed[player->EntIndex()] = accel * wishspeed * GLOBALS->frametime * player->m_surfaceFriction();

	// Cap it
	if (accelspeed[player->EntIndex()] > addspeed[player->EntIndex()])
		accelspeed[player->EntIndex()] = addspeed[player->EntIndex()];

	// Adjust pmove vel.
	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<3; i[player->EntIndex()]++)
	{
		player->GetVelocity()[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];
		Interface->MoveHelper()->SetHost(player);
		Interface->MoveHelper()->m_outWishVel[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];

	}
}

void RebuildGameMovement::AirMove(C_CSPlayer* player)
{
	Math::AngleVectors(player->GetEyeAngles(), forward[player->EntIndex()], right[player->EntIndex()], up[player->EntIndex()]);  // Determine movement angles

																																   // Copy movement amounts
	Interface->MoveHelper()->SetHost(player);
	fmove[player->EntIndex()] = Interface->MoveHelper()->m_flForwardMove;
	smove[player->EntIndex()] = Interface->MoveHelper()->m_flSideMove;

	// Zero out z components of movement vectors
	forward[player->EntIndex()][2] = 0;
	right[player->EntIndex()][2] = 0;
	NormalizeVector(forward[player->EntIndex()]);  // Normalize remainder of vectors
	NormalizeVector(right[player->EntIndex()]);    // 

	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<2; i[player->EntIndex()]++)       // Determine x and y parts of velocity
		wishvel[player->EntIndex()][i[player->EntIndex()]] = forward[player->EntIndex()][i[player->EntIndex()]] * fmove[player->EntIndex()] + right[player->EntIndex()][i[player->EntIndex()]] * smove[player->EntIndex()];

	wishvel[player->EntIndex()][2] = 0;             // Zero out z part of velocity

	wishdir[player->EntIndex()] = wishvel[player->EntIndex()]; // Determine maginitude of speed of move
	wishspeed[player->EntIndex()] = wishdir[player->EntIndex()].Normalize2();

	//
	// clamp to server defined max speed
	//
	if (wishspeed != 0 && (wishspeed[player->EntIndex()] > player->m_flMaxspeed()))
	{
		VectorMultiply(wishvel[player->EntIndex()], player->m_flMaxspeed() / wishspeed[player->EntIndex()], wishvel[player->EntIndex()]);
		wishspeed[player->EntIndex()] = player->m_flMaxspeed();
	}

	AirAccelerate(player, wishdir[player->EntIndex()], wishspeed[player->EntIndex()], CVAR->FindVar(XorStr("sv_airaccelerate"))->GetFloat());

	// Add in any base velocity to the current velocity.
	Math::VectorAdd(player->GetVelocity(), player->m_vecBaseVelocity(), player->GetVelocity());
	trace_t trace;
	TryPlayerMove(player, &dest[player->EntIndex()], &trace);

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	Math::VectorSubtract(player->GetVelocity(), player->m_vecBaseVelocity(), player->GetVelocity());
}

void RebuildGameMovement::StepMove(C_CSPlayer* player, Vector &vecDestination, trace_t &trace)
{
	Vector vecEndPos = vecDestination;

	Vector vecPos = player->GetAbsOrigin();
	Vector vecVel = player->GetVelocity();

	// Slide move down.
	TryPlayerMove(player, &vecEndPos, &trace);

	Vector vecDownPos = player->GetAbsOrigin();
	Vector vecDownVel = player->GetVelocity();

	// Reset original values.
	SetAbsOrigin(player, vecPos);
	player->GetVelocity() = vecVel;

	// Move up a stair height.
	vecEndPos = player->GetAbsOrigin();

	vecEndPos.z += player->GetStepSize() + 0.03125;


	TracePlayerBBox(player->GetAbsOrigin(), vecEndPos, MASK_PLAYERSOLID, 8, trace, player);
	if (!trace.startsolid && !trace.allsolid)
	{
		SetAbsOrigin(player, trace.endpos);
	}

	TryPlayerMove(player, &dest[player->EntIndex()], &trace);

	// Move down a stair (attempt to).
	vecEndPos = player->GetAbsOrigin();

	vecEndPos.z -= player->GetStepSize() + 0.03125;


	TracePlayerBBox(player->GetAbsOrigin(), vecEndPos, MASK_PLAYERSOLID, 8, trace, player);

	// If we are not on the ground any more then use the original movement attempt.
	if (trace.plane.normal[2] < 0.7)
	{
		SetAbsOrigin(player, vecDownPos);
		player->GetVelocity() = vecDownVel;

		float flStepDist = player->GetAbsOrigin().z - vecPos.z;
		if (flStepDist > 0.0f)
		{
			Interface->MoveHelper()->SetHost(player);
			Interface->MoveHelper()->m_outStepHeight += flStepDist;
			Interface->MoveHelper()->SetHost(nullptr);
		}
		return;
	}

	// If the trace ended up in empty space, copy the end over to the origin.
	if (!trace.startsolid && !trace.allsolid)
	{
		player->SetAbsOrigin(trace.endpos);
	}

	Vector vecUpPos = player->GetAbsOrigin();

	// decide which one went farther
	float flDownDist = (vecDownPos.x - vecPos.x) * (vecDownPos.x - vecPos.x) + (vecDownPos.y - vecPos.y) * (vecDownPos.y - vecPos.y);
	float flUpDist = (vecUpPos.x - vecPos.x) * (vecUpPos.x - vecPos.x) + (vecUpPos.y - vecPos.y) * (vecUpPos.y - vecPos.y);
	if (flDownDist > flUpDist)
	{
		SetAbsOrigin(player, vecDownPos);
		player->GetVelocity() = vecDownVel;
	}
	else
	{
		// copy z value from slide move
		player->GetVelocity() = vecDownVel;
	}

	float flStepDist = player->GetAbsOrigin().z - vecPos.z;
	if (flStepDist > 0)
	{
		Interface->MoveHelper()->SetHost(player);
		Interface->MoveHelper()->m_outStepHeight += flStepDist;
		Interface->MoveHelper()->SetHost(nullptr);
	}
}

void RebuildGameMovement::TracePlayerBBox(const Vector &start, const Vector &end, unsigned int fMask, int collisionGroup, trace_t& pm, C_CSPlayer* player)
{
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = reinterpret_cast<void*>(player);

	ray.Init(start, end, player->GetCollideable()->OBBMins(), player->GetCollideable()->OBBMaxs());
	ENGINETRACE->TraceRay(ray, fMask, &filter, &pm);
}

void RebuildGameMovement::WalkMove(C_CSPlayer* player)
{
	Math::AngleVectors(player->GetEyeAngles(), forward[player->EntIndex()], right[player->EntIndex()], up[player->EntIndex()]);  // Determine movement angles
																																   // Copy movement amounts
	Interface->MoveHelper()->SetHost(player);
	fmove[player->EntIndex()] = Interface->MoveHelper()->m_flForwardMove;
	smove[player->EntIndex()] = Interface->MoveHelper()->m_flSideMove;
	Interface->MoveHelper()->SetHost(nullptr);


	if (forward[player->EntIndex()][2] != 0)
	{
		forward[player->EntIndex()][2] = 0;
		NormalizeVector(forward[player->EntIndex()]);
	}

	if (right[player->EntIndex()][2] != 0)
	{
		right[player->EntIndex()][2] = 0;
		NormalizeVector(right[player->EntIndex()]);
	}


	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<2; i[player->EntIndex()]++)       // Determine x and y parts of velocity
		wishvel[player->EntIndex()][i[player->EntIndex()]] = forward[player->EntIndex()][i[player->EntIndex()]] * fmove[player->EntIndex()] + right[player->EntIndex()][i[player->EntIndex()]] * smove[player->EntIndex()];

	wishvel[player->EntIndex()][2] = 0;             // Zero out z part of velocity

	wishdir[player->EntIndex()] = wishvel[player->EntIndex()]; // Determine maginitude of speed of move
	wishspeed[player->EntIndex()] = wishdir[player->EntIndex()].Normalize2();

	//
	// Clamp to server defined max speed
	//
	Interface->MoveHelper()->SetHost(player);
	if ((wishspeed[player->EntIndex()] != 0.0f) && (wishspeed[player->EntIndex()] > Interface->MoveHelper()->m_flMaxSpeed))
	{
		VectorMultiply(wishvel[player->EntIndex()], player->m_flMaxspeed() / wishspeed[player->EntIndex()], wishvel[player->EntIndex()]);
		wishspeed[player->EntIndex()] = player->m_flMaxspeed();
	}
	Interface->MoveHelper()->SetHost(nullptr);
	// Set pmove velocity
	player->GetVelocity()[2] = 0;
	Accelerate(player, wishdir[player->EntIndex()], wishspeed[player->EntIndex()], CVAR->FindVar(XorStr("sv_accelerate"))->GetFloat());
	player->GetVelocity()[2] = 0;

	// Add in any base velocity to the current velocity.
	Math::VectorAdd(player->GetVelocity(), player->m_vecBaseVelocity(), player->GetVelocity());

	spd[player->EntIndex()] = player->GetVelocity().Length();

	if (spd[player->EntIndex()] < 1.0f)
	{
		player->GetVelocity().Zero();
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		Math::VectorSubtract(player->GetVelocity(), player->m_vecBaseVelocity(), player->GetVelocity());
		return;
	}

	// first try just moving to the destination	
	dest[player->EntIndex()][0] = player->GetAbsOrigin()[0] + player->GetVelocity()[0] * GLOBALS->frametime;
	dest[player->EntIndex()][1] = player->GetAbsOrigin()[1] + player->GetVelocity()[1] * GLOBALS->frametime;
	dest[player->EntIndex()][2] = player->GetAbsOrigin()[2];

	// first try moving directly to the next spot
	TracePlayerBBox(player->GetAbsOrigin(), dest[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);

	// If we made it all the way, then copy trace end as new player position.
	Interface->MoveHelper()->SetHost(player);
	Interface->MoveHelper()->m_outWishVel += wishdir[player->EntIndex()] * wishspeed[player->EntIndex()];
	Interface->MoveHelper()->SetHost(nullptr);

	if (pm[player->EntIndex()].fraction == 1)
	{
		player->SetAbsOrigin(pm[player->EntIndex()].endpos);
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		Math::VectorSubtract(player->GetVelocity(), player->m_vecBaseVelocity(), player->GetVelocity());

		return;
	}

	// Don't walk up stairs if not on ground.
	if (!(player->GetFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		Math::VectorSubtract(player->GetVelocity(), player->m_vecBaseVelocity(), player->GetVelocity());
		return;
	}

	StepMove(player, dest[player->EntIndex()], pm[player->EntIndex()]);

	// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	Math::VectorSubtract(player->GetVelocity(), player->m_vecBaseVelocity(), player->GetVelocity());


}

void RebuildGameMovement::FinishGravity(C_CSPlayer* player)
{
	float ent_gravity;

	ent_gravity = CVAR->FindVar(XorStr("sv_gravity"))->GetFloat();

	// Get the correct velocity for the end of the dt 
	player->GetVelocity()[2] -= (ent_gravity * CVAR->FindVar(XorStr("sv_gravity"))->GetFloat() * GLOBALS->frametime * 0.5);

	CheckVelocity(player);
}

void RebuildGameMovement::FullWalkMove(C_CSPlayer* player)
{

	StartGravity(player);

	// Fricion is handled before we add in any base velocity. That way, if we are on a conveyor, 
	//  we don't slow when standing still, relative to the conveyor.
	if (player->GetFlags() & FL_ONGROUND)
	{
		player->GetVelocity()[2] = 0.0;
		Friction(player);
	}

	// Make sure velocity is valid.
	CheckVelocity(player);

	if (player->GetFlags() & FL_ONGROUND)
	{
		WalkMove(player);
	}
	else
	{
		AirMove(player);  // Take into account movement when in air.
	}

	// Make sure velocity is valid.
	CheckVelocity(player);

	// Add any remaining gravitational component.
	FinishGravity(player);


	// If we are on ground, no downward velocity.
	if (player->GetFlags() & FL_ONGROUND)
	{
		player->GetVelocity()[2] = 0;
	}

	CheckFalling(player);
}

void RebuildGameMovement::Friction(C_CSPlayer* player)
{
	// Calculate speed
	speed[player->EntIndex()] = player->GetVelocity().Length();

	// If too slow, return
	if (speed[player->EntIndex()] < 0.1f)
	{
		return;
	}

	drop[player->EntIndex()] = 0;

	// apply ground friction
	if (player->GetFlags() & FL_ONGROUND)  // On an C_CSPlayer that is the ground
	{
		friction[player->EntIndex()] = CVAR->FindVar(XorStr("sv_friction"))->GetFloat() * player->m_surfaceFriction();

		//  Bleed off some speed, but if we have less than the bleed
		//  threshold, bleed the threshold amount.


		control[player->EntIndex()] = (speed[player->EntIndex()] < CVAR->FindVar(XorStr("sv_stopspeed"))->GetFloat()) ? CVAR->FindVar(XorStr("sv_stopspeed"))->GetFloat() : speed[player->EntIndex()];

		// Add the amount to the drop amount.
		drop[player->EntIndex()] += control[player->EntIndex()] * friction[player->EntIndex()] * GLOBALS->frametime;
	}

	// scale the velocity
	newspeed[player->EntIndex()] = speed[player->EntIndex()] - drop[player->EntIndex()];
	if (newspeed[player->EntIndex()] < 0)
		newspeed[player->EntIndex()] = 0;

	if (newspeed[player->EntIndex()] != speed[player->EntIndex()])
	{
		// Determine proportion of old speed we are using.
		newspeed[player->EntIndex()] /= speed[player->EntIndex()];
		// Adjust velocity according to proportion.
		VectorMultiply(player->GetVelocity(), newspeed[player->EntIndex()], player->GetVelocity());
	}

	player->GetVelocity() -= player->GetVelocity() * (1.f - newspeed[player->EntIndex()]);
}


void RebuildGameMovement::CheckFalling(C_CSPlayer* player)
{
	// this function really deals with landing, not falling, so early out otherwise
	if (player->GetFallVelocity() <= 0)
		return;

	if (!player->GetHealth() && player->GetFallVelocity() >= 303.0f)
	{
		bool bAlive = true;
		float fvol = 0.5;

		//
		// They hit the ground.
		//
		if (player->GetVelocity().z < 0.0f)
		{
			// Player landed on a descending object. Subtract the velocity of the ground C_CSPlayer.
			player->GetFallVelocity() += player->GetVelocity().z;
			player->GetFallVelocity() = max(0.1f, player->GetFallVelocity());
		}

		if (player->GetFallVelocity() > 526.5f)
		{
			fvol = 1.0;
		}
		else if (player->GetFallVelocity() > 526.5f / 2)
		{
			fvol = 0.85;
		}
		else if (player->GetFallVelocity() < 173)
		{
			fvol = 0;
		}

	}

	// let any subclasses know that the player has landed and how hard

	//
	// Clear the fall velocity so the impact doesn't happen again.
	//
	player->GetFallVelocity() = 0;
}
const int nanmask = 255 << 23;
#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)
void RebuildGameMovement::CheckVelocity(C_CSPlayer* player)
{
	Vector org = player->GetAbsOrigin();

	for (i[player->EntIndex()] = 0; i[player->EntIndex()] < 3; i[player->EntIndex()]++)
	{
		// See if it's bogus.
		if (IS_NAN(player->GetVelocity()[i[player->EntIndex()]]))
		{
			player->GetVelocity()[i[player->EntIndex()]] = 0;
		}

		if (IS_NAN(org[i[player->EntIndex()]]))
		{
			org[i[player->EntIndex()]] = 0;
			player->SetAbsOrigin(org);
		}

		// Bound it.
		if (player->GetVelocity()[i[player->EntIndex()]] > CVAR->FindVar(XorStr("sv_maxvelocity"))->GetFloat())
		{
			player->GetVelocity()[i[player->EntIndex()]] = CVAR->FindVar(XorStr("sv_maxvelocity"))->GetFloat();
		}
		else if (player->GetVelocity()[i[player->EntIndex()]] < -CVAR->FindVar(XorStr("sv_maxvelocity"))->GetFloat())
		{
			player->GetVelocity()[i[player->EntIndex()]] = -CVAR->FindVar(XorStr("sv_maxvelocity"))->GetFloat();
		}
	}
}
void RebuildGameMovement::StartGravity(C_CSPlayer* player)
{
	if (!player || !player->GetHealth())
		return;

	Vector pVel = player->GetVelocity();

	pVel[2] -= (CVAR->FindVar(XorStr("sv_gravity"))->GetFloat() * 0.5f * GLOBALS->interval_per_tick);
	pVel[2] += (player->m_vecBaseVelocity()[2] * GLOBALS->interval_per_tick);

	player->GetVelocity() = pVel;

	Vector tmp = player->m_vecBaseVelocity();
	tmp[2] = 0.f;
	player->GetVelocity() = tmp;
}
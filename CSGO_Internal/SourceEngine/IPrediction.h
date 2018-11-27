#pragma once

class CMoveData
{
public:

	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;
	bool m_bNoAirControl : 1;

	void *m_nPlayerHandle;    // edict index on server, client entity handle on client

	int m_nImpulseCommand;    // Impulse command issued.
	Vector m_vecViewAngles;    // Command view angles (local space)
	Vector m_vecAbsViewAngles;    // Command view angles (world space)
	int m_nButtons;            // Attack buttons.
	int m_nOldButtons;        // From host_client->oldbuttons;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;

	float m_flMaxSpeed;
	float m_flClientMaxSpeed;

	// Variables from the player edict (sv_player) or entvars on the client.
	// These are copied in here before calling and copied out after calling.
	Vector m_vecVelocity;        // edict::velocity        // Current movement direction.
	Vector m_vecOldVelocity;
	float somefloat;
	Vector m_vecAngles;        // edict::angles
	Vector m_vecOldAngles;

	// Output only
	float m_outStepHeight;    // how much you climbed this move
	Vector m_outWishVel;        // This is where you tried 
	Vector m_outJumpVel;        // This is your jump velocity
								// Movement constraints    (radius 0 means no constraint)
	Vector m_vecConstraintCenter;
	float m_flConstraintRadius;
	float m_flConstraintWidth;
	float m_flConstraintSpeedFactor;
	bool m_bConstraintPastRadius;        ///< If no, do no constraining past Radius.  If yes, cap them to SpeedFactor past radius

	void SetAbsOrigin(const Vector &vec)
	{
		m_vecAbsOrigin = vec;
	}
	const Vector &GetAbsOrigin() const
	{
		return m_vecAbsOrigin;
	}

private:
	Vector m_vecAbsOrigin;        // edict::origin
};

class IMoveHelper
{
public:

	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;
	int m_nPlayerHandle; // edict index on server, client entity handle on client=
	int m_nImpulseCommand; // Impulse command issued.
	Vector m_vecViewAngles; // Command view angles (local space)
	Vector m_vecAbsViewAngles; // Command view angles (world space)
	int m_nButtons; // Attack buttons.
	int m_nOldButtons; // From host_client->oldbuttons;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;
	float m_flMaxSpeed;
	float m_flClientMaxSpeed;
	Vector m_vecVelocity; // edict::velocity // Current movement direction.
	Vector m_vecAngles; // edict::angles
	Vector m_vecOldAngles;
	float m_outStepHeight; // how much you climbed this move
	Vector m_outWishVel; // This is where you tried 
	Vector m_outJumpVel; // This is your jump velocity
	Vector m_vecConstraintCenter;
	float m_flConstraintRadius;
	float m_flConstraintWidth;
	float m_flConstraintSpeedFactor;
	float m_flUnknown[5];
	Vector m_vecAbsOrigin;

	void SetHost(void *host)
	{
		typedef void(__thiscall *o_SetHost)(void*, void*);
		return CallVFunction<o_SetHost>(this, 1)(this, host);
	}
};

class IGameMovement
{
public:

	void ProcessMovement(IClientEntity *pPlayer, void *pMove)
	{
		typedef void(__thiscall* oProcessMovement)(PVOID, IClientEntity*, void*);
		return CallVFunction< oProcessMovement >(this, 1)(this, pPlayer, pMove);
	}

	void StartTrackPredictionErrors(IClientEntity* player)
	{
		typedef void(__thiscall* oPredictionError)(PVOID, IClientEntity*);
		return CallVFunction< oPredictionError >(this, 3)(this, player);
	}

	void FinishTrackPredictionErrors(IClientEntity* player)
	{
		typedef void(__thiscall* oPredictionError)(PVOID, IClientEntity*);
		return CallVFunction< oPredictionError >(this, 4)(this, player);
	}
};

class IPrediction
{
public:

	/*~CPrediction()																			0
	Init(void)																					1
	Shutdown(void)																				2
	Update(int, bool, int, int)																	3
	PreEntityPacketReceived(int, int, int)														4
	PostEntityPacketReceived(void)																5
	PostNetworkDataReceived(int)																6
	OnReceivedUncompressedPacket(void)															7
	GetViewOrigin(Vector &)																		8
	SetViewOrigin(Vector &)																		9
	GetViewAngles(QAngle &)																		10
	SetViewAngles(QAngle &)																		11
	GetLocalViewAngles(QAngle &)																12
	SetLocalViewAngles(QAngle &)																13
	InPrediction(void)const																		14
	IsFirstTimePreicted(void)const																15
	GetLastAcknowledgedCommandNumber(void)const													16
	GetIncomingPacketNumber(void)const															17
	CheckMovingGround(C_BasePlayer *, double)													18
	RunCommand(C_BasePlayer *, CUserCmd *, IMoveHelper *)										19
	SetupMove(C_BasePlayer *, CUserCmd *, IMoveHelper *, CMoveData *)							20
	FinishMove(C_BasePlayer *, CUserCmd *, CMoveData *)											21
	SetIdealPitch(int, C_BasePlayer *, Vector const&, QAngle const&, Vector const&)				22
	CheckError(int, C_BasePlayer *, int)														23
	_Update(int, bool, bool, int, int)															24
	*/

	void GetLocalViewAngles(QAngle& angle) {
		typedef void(__thiscall* oFinishMove)(PVOID, QAngle&);
		return CallVFunction< oFinishMove >(this, 12)(this, angle);
	}

	void SetLocalViewAngles(QAngle& angle) {
		typedef void(__thiscall* oFinishMove)(PVOID, QAngle&);
		return CallVFunction< oFinishMove >(this, 13)(this, angle);
	}

	bool InPrediction() {
		typedef bool(__thiscall* oInPrediction)(PVOID);
		return CallVFunction< oInPrediction >(this, 14)(this);
	}

	void RunCommand(IClientEntity *player, CUserCmd *ucmd, IMoveHelper *moveHelper) {
		typedef void(__thiscall* oRunCommand)(PVOID, IClientEntity*, CUserCmd*, IMoveHelper*);
		return CallVFunction< oRunCommand >(this, 19)(this, player, ucmd, moveHelper);
	}

	void SetupMove(IClientEntity *player, CUserCmd *ucmd, IMoveHelper *moveHelper, void*pMoveData) {
		typedef void(__thiscall* oSetupMove)(PVOID, IClientEntity*, CUserCmd*, IMoveHelper*, void*);
		return CallVFunction< oSetupMove >(this, 20)(this, player, ucmd, moveHelper, pMoveData);
	}

	void FinishMove(IClientEntity *player, CUserCmd *ucmd, void* pMoveData) {
		typedef void(__thiscall* oFinishMove)(PVOID, IClientEntity*, CUserCmd*, void*);
		return CallVFunction< oFinishMove >(this, 21)(this, player, ucmd, pMoveData);
	}
};

//class RebuildGameMovement
//{
//private:
//
//	// ~ deleted cause pub release
//
//private:
//
//public:
//
//	RebuildGameMovement(void);
//
//	void SetAbsOrigin(Entity *entity, const Vector3 &vec);
//	void FullWalkMove(Entity *entity);
//	void CheckVelocity(Entity *entity);
//	void FinishGravity(Entity *entity);
//	void StepMove(Entity *entity, Vector3 &vecDestination, trace_t &trace);
//	int ClipVelocity(Vector3 &in, Vector3 &normal, Vector3 &out, float overbounce);
//	int TryPlayerMove(Entity *entity, Vector3 *pFirstDest, trace_t *pFirstTrace);
//	void Accelerate(Entity *entity, Vector3 &wishdir, float wishspeed, float accel);
//	void Friction(Entity *entity);
//	void AirAccelerate(Entity *entity, Vector3 &wishdir, float wishspeed, float accel);
//	void AirMove(Entity *entity);
//	void WalkMove(Entity *entity);
//	void CheckFalling(Entity *entity);
//	void StartGravity(Entity *entity);
//	void TracePlayerBBox(const Vector3 &start, const Vector3 &end, unsigned int fMask, int collisionGroup, trace_t &pm, Entity *entity);
//};

//extern RebuildGameMovement *rebGameMovement;
//extern MoveHelper *p_MoveHelper;
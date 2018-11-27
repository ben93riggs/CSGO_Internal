#pragma once
#include "Singleton.h"
#include "SourceEngine/SDK.hpp"

struct BacktrackedPlayer;

struct Record {
	float m_flSimulationTime;
	float m_flCycle;
	int m_nSequence;
	int m_nFlags;
	Vector m_vecOrigin;
	QAngle m_angAngles;
	Vector m_vecMins;
	Vector m_vecMaxs;
	std::array<float, 24> m_arrflPoseParameters{};
	std::array<C_AnimationLayer, 15> m_LayerRecords{};
	float m_flLowerBodyYawTarget;
	int m_nLbyUpdated;
	Vector m_vecHeadSpot;
	bool m_bMatrixBuilt;
	matrix3x4_t m_BoneMatrix[MAXSTUDIOBONES];
	Record(){}
	Record(C_CSPlayer* pEntity, BacktrackedPlayer& Player);
};

struct BacktrackedPlayer {
	int m_nLbyCorrect;
	int m_nBruteforce;
	bool m_bBacktracked;
	int m_bBacktrackedToLby;
	float m_flSimulationTime;
	float m_flPrevSimulationTime;
	float m_flCycle;
	int m_nSequence;
	int m_nFlags;
	Vector m_vecOrigin;
	Vector m_vecAbsOrgin;
	QAngle m_angAngles;
	Vector m_angAbsAngles;
	Vector m_vecMins;
	Vector m_vecMaxs;
	std::array<float, 24> m_arrflPoseParameters;
	std::array<C_AnimationLayer, 15> m_LayerRecords;	
	float m_flLowerBodyYawTarget;
	float m_flLbyUpdate;
	int m_nLbyUpdated;
	bool m_bShouldBaim = false;
	bool m_bLbyTimingAccurate;
	int m_nSelectedTick;
	std::vector<Record> m_LagRecord;
	bool Cache(C_CSPlayer *pEntity);
	bool m_bInitialized = false;
	bool m_bUpdated = false;
	CBaseHandle m_ulEntHandle;
	float m_flSpawnTime;
};

struct Shot_t {
	int m_bLbyUpdate = 0;
	int m_nTargetIndex = -1;
	Vector m_EyePos = Vector(0,0,0);
	matrix3x4_t m_BoneMatrix[128];
	int m_nWritableBones = 0;
	int m_nReadableBones = 0;
};

class Accuracy : public singleton<Accuracy> {
	bool m_bShotReceived = false;
	Vector m_Impact = Vector();
	int m_nHurtIndex = -1;
	Shot_t m_LastShot;
	
	bool MissedDueToSpread();
	bool SelectTick(C_CSPlayer *pEntity, BacktrackedPlayer &Player);
	bool Predict(C_CSPlayer *pEntity, BacktrackedPlayer &Player);

public:
	void StorePlayers();
	BacktrackedPlayer Players[65];

	static float GetLerpTime();
	bool is_tick_valid(float tick);
	Record& GetLastValidRecord(int index);
	bool TraceToExitShort(Vector& point, Vector& dir, float flStepSize, float flMaxDistance);
	float GetThickness(Vector& start, Vector& end);
	void AddShot(C_CSPlayer* pLocal, C_CSPlayer* pEntity);
	void PlayerHurt(C_CSPlayer *pEntity);
	void ShotReceived(Vector impact);
	void BacktrackPlayer(C_CSPlayer* pEntity, BacktrackedPlayer& Player);
	void BacktrackPlayerToTick(C_CSPlayer *pEntity, BacktrackedPlayer &Player, Record &restore);
	void Resolve(C_CSPlayer *pEntity, BacktrackedPlayer &Player);
	void BacktrackLegit(C_CSPlayer* pLocal, C_CSPlayer* pEntity);
	void Backtrack();
	bool SelectTickLegit(C_CSPlayer* pLocal, C_CSPlayer* pEntity, BacktrackedPlayer& Player);
	void FrameStageNotify(ClientFrameStage_t stage);
	void RestorePlayers();
	void RestorePlayer(C_CSPlayer *pEntity, BacktrackedPlayer &Player);
};
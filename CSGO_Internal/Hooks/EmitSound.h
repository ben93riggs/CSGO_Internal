#pragma once
#include "SourceEngine/IEngineSound.h"
#include "../SoundESP.h"

using EmitSoundAtt_t = void(__thiscall*)(IEngineSound*, IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector *pOrigin, const Vector *pDirection, CUtlVector<Vector>* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unknown);
EmitSoundAtt_t g_fnOriginalEmitSound;

enum SoundFlags_t
{
	SND_NOFLAGS = 0,			// to keep the compiler happy
	SND_CHANGE_VOL = (1 << 0),		// change sound vol
	SND_CHANGE_PITCH = (1 << 1),		// change sound pitch
	SND_STOP = (1 << 2),		// stop the sound
	SND_SPAWNING = (1 << 3),		// we're spawning, used in some cases for ambients
	// not sent over net, only a param between dll and server.
	SND_DELAY = (1 << 4),		// sound has an initial delay
	SND_STOP_LOOPING = (1 << 5),		// stop all looping sounds on the entity.
	SND_SPEAKER = (1 << 6),		// being played again by a microphone through a speaker

	SND_SHOULDPAUSE = (1 << 7),		// this sound should be paused if the game is paused
	SND_IGNORE_PHONEMES = (1 << 8),
	SND_IGNORE_NAME = (1 << 9),		// used to change all sounds emitted by an entity, regardless of scriptname

	SND_DO_NOT_OVERWRITE_EXISTING_ON_CHANNEL = (1 << 10),
};

inline void __fastcall Hooked_EmitSound(IEngineSound* ecx, void* edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector *pOrigin, const Vector *pDirection, CUtlVector<Vector>* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unknown)
{
	if (!MenuVars::get().g_bEmitSound)
		return g_fnOriginalEmitSound(ecx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unknown);

	//if (strstr(pSample, XorStr("null")))
	//{
	//	//DEBUGOVERLAY->AddTextOverlay(*pOrigin, .1f, XorStr("LAGGER_DETECTED"));
	//	//C_CSPlayer* pEnt = C_CSPlayer::GetPlayerByIndex(ENGINE->GetPlayerForUserID(iEntIndex));
	//	//if (pEnt)
	//	//{
	//	//	std::string temp = pEnt->sGetName() + XorStr(" is using a null sound exploit!");
	//	//	ENGINE->ClientCmd(temp.c_str());
	//	//}
	//	pSample = "";
	//	iPitch = 256;
	//	iFlags = SND_STOP | SND_STOP_LOOPING; // (1 << 2) | (1 << 5)
	//}

	if (!ENGINE->IsInGame() || !ENGINE->IsConnected())
		return g_fnOriginalEmitSound(ecx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unknown);

	SoundESP::get().EmitSound(iEntIndex, pOrigin);
	
	return g_fnOriginalEmitSound(ecx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unknown);
}
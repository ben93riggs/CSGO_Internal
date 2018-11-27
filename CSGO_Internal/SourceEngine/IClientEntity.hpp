#pragma once

#include "SDK.hpp"
#include"IClientNetworkable.hpp"
#include"IClientRenderable.hpp"
#include"IClientUnknown.hpp"
#include"IClientThinkable.hpp"

#define VirtualFn( cast )typedef cast( __thiscall* OriginalFn )

class CBaseCombatWeapon;
class CBaseViewModel;

     struct SpatializationInfo_t;

	 class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
	 {

	 public:
		 virtual void             Release(void) = 0;
		 virtual const Vector&    oGetAbsOrigin(void) const = 0;
		 virtual const QAngle&    GetAbsAngles(void) const = 0;
		 virtual void*            GetMouth(void) = 0;
		 virtual bool             GetSoundSpatialization(SpatializationInfo_t info) = 0;
		 virtual bool             IsBlurred(void) = 0;
	 };

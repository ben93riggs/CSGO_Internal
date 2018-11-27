#pragma once

class CCSWeaponInfo
{
public:
	char pad_0x0000[0x4]; //0x0000 <- virtual destructor
	char * consoleName; //0x0004 
	char pad_0x0008[0xC]; //0x0008
	__int32 iMaxClip1; //0x0014 
	char pad_0x0018[0xC]; //0x0018
	__int32 iMaxClip2; //0x0024 
	char pad_0x0028[0x4]; //0x0028
	char * szWorldModel; //0x002C 
	char * szViewModel; //0x0030 
	char * szDroppedModel; //0x0034 
	char pad_0x0038[0x4]; //0x0038
	char * szUnknownName; //0x003C 
	char pad_0x0040[0x38]; //0x0040
	char * szEmptySound; //0x0078 
	char pad_0x007C[0x4]; //0x007C
	char * szBulletType; //0x0080 
	char pad_0x0084[0x4]; //0x0084
	char * szHudName; //0x0088 
	char * szWeaponName; //0x008C 
	char pad_0x0090[0x3C]; //0x0090 
	__int32 iWeaponType; //0x00CC 
	__int32 iWeaponPrice; //0x00D0 
	__int32 iKillAward; //0x00D4 
	char * szAnimationPrefix; //0x00D8 
	float flCycleTime; //0x00DC 
	float flCycleTimeAlt; //0x00E0 
	float flTimeToIdle; //0x00E4 
	float flIdleInterval; //0x00E8 
	__int8 bFullAuto; //0x00EC
	char pad_0x00E9[0x3]; //0x00ED
	__int32 iDamage; //0x00F0 
	float flArmorRatio; //0x00F4 
	__int32 iBullets; //0x00F8
	float flPenetration; //0x00FC 
	float flFlinchVelocityModifierLarge; //0x0100
	float flFlinchVelocityModifierSmall; //0x0104 
	float flRange; //0x0108
	float flRangeModifier; //0x010C 
	float flThrowVelocity; //0x0110 
	char pad_0x0110[0xC]; //0x0114
	__int8 bSuppressed; //0x0120 
	char pad_0x011D[0x3]; //0x0121
	char * pSilencerModel; //0x0124 
	char pad_0x0124[0x4]; //0x0128
	__int32 iTeamNum; //0x012C
	float flMaxSpeed; //0x0130 
	float flMaxSpeed2; //0x0134 
	float flSpread; //0x0138
	float flSpreadAlt; //0x013C 
	float flInaccuracyCrouch; //0x0140 
	float flInaccuracyCrouchAlt; //0x0144 
	float flInaccuracyStand; //0x0148
	float flInaccuracyStandAlt; //0x014C i left off changing the offset comments im lazy
	float flInaccuracyJumpInitial; //0x014C 
	float flInaccuracyJump; //0x0150 
	float flInaccuracyJumpAlt; //0x0154 
	float flInaccuracyLand; //0x0158 
	float flInaccuracyLandAlt; //0x015C 
	float flInaccuracyLadder; //0x0160 
	float flInaccuracyLadderAlt; //0x0164 
	float flInaccuracyFire; //0x0168 
	float flInaccuracyFireAlt; //0x016C 
	float flInaccuracyMove; //0x0170 
	float flInaccuracyMoveAlt; //0x0174 
	float flInaccuracyReload; //0x0178 
	float flShotgunSpreadIDKIFTHISISRIGHT; //0x017C 
	__int32 iRecoilSeed; //0x0180 
	float flRecoilAngle; //0x0184 
	float flRecoilAngleAlt; //0x0188 
	float flRecoilAngleVariance; //0x018C 
	float flRecoilAngleVarianceAlt; //0x0190 
	float flRecoilMagnitude; //0x0194 
	float flRecoilMagnitudeAlt; //0x0198 
	float flRecoilMagnitudeVariance; //0x019C 
	float flRecoilMagnitudeVarianceAlt; //0x01A0 
	float flRecoveryTimeCrouch; //0x01A4 
	float flRecoveryTimeStand; //0x01A8 
	float flRecoveryTimeCrouchFinal; //0x01AC 
	float flRecoveryTimeStandFinal; //0x01B0 
	__int32 iRecoveryTransitionStartBullet; //0x01B4 
	__int32 iRecoveryTransitionEndBullet; //0x01B8 
	__int8 bUnzoomAfterShot; //0x01BC 
	__int8 bHideViewModelZoomed; //0x01BD 
	char pad_0x01BE[0x2]; //0x01BE
	__int32 iZoomLevels; //0x01C0 
	__int32 iZoomFOV; //0x01C4 
	__int32 iZoomFOV2; //0x01C8 
	float flZoomTime; //0x01CC 
	float flZoomTime2; //0x01D0 
	float flZoomTime3; //0x01D4 
	char * szWeaponClass; //0x01D8 
	float flAddonScale; //0x01DC 
	char pad_0x01E0[0x4]; //0x01E0
	char * szEjectBrassEffect; //0x01E4 
	char * szTracerEffect; //0x01E8 
	__int32 iTracerFrequency; //0x01EC 
	__int32 iTracerFrequencyAlt; //0x01F0 
	char * szMuzzleFlashEffect_1stPerson; //0x01F4 
	char pad_0x01F8[0x4]; //0x01F8
	char * szMuzzleFlashEffect_3rdPerson; //0x01FC 
	char pad_0x0200[0x4]; //0x0200
	char * szMuzzleSmokeEffect; //0x0204 
	float flHeatPerShot; //0x0208 
	char * szZoomInSound; //0x020C 
	char * szZoomOutSound; //0x0210 
	float flInaccuracyPitchShift; //0x0214 
	float flInaccuracySoundThreshold; //0x0218 
	float flBotAudibleRange; //0x021C 
	char pad_0x0220[0x8]; //0x0220
	char * pWrongTeamMsg; //0x0228 
	__int8 bHasBurstMode; //0x022C 
	char pad_0x022D[0x3]; //0x022D
	__int8 bIsRevolver; //0x0230 
	char pad_0x0231[0x3]; //0x0231
	__int8 bCannotShootUnderwater; //0x0234 
};

class IWeaponSystem
{
	virtual void unused0() = 0;
	virtual void unused1() = 0;
public:
	virtual const CCSWeaponInfo* GetWpnData(unsigned ItemDefinitionIndex) = 0;
};

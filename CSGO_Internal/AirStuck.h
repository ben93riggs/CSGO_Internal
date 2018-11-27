#pragma once
//
//namespace AirStuck
//{
//	inline void CreateMove()
//	{
//		C_CSPlayer* pLocal = C_CSPlayer::GetLocalPlayer();
//
//		if (!pLocal)
//			return;
//
//		if (!MiscVars::get().g_bAirStuck)
//			return;
//
//		if (!ENGINE->IsInGame())
//			return;
//
//		if (G::aimbotting && pLocal->bCanFire()) 
//			return;
//
//		if (INPUTSYSTEM->IsButtonDown(MenuVars::get().g_kAirStuckKey))
//		{
//			G::UserCmd->tick_count = 0xFFFFF;// 16777216;
//		}
//	}
//};

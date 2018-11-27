//#include "NoSpread.h"
//#include "Utils.hpp"
//
//#include "checksum_md5.h"
//
//CNoSpread NoSpread;
//
//C_Random* Random = new C_Random;
//
//void C_Random::SetSeed(int iSeed)
//{
//	m_idum = ((iSeed < 0) ? iSeed : -iSeed);
//	m_iy = 0;
//}
//
//int C_Random::GenerateRandomNumber()
//{
//	int j;
//	int k;
//
//	if (m_idum <= 0 || !m_iy) {
//		if (-(m_idum) < 1) m_idum = 1;
//		else m_idum = -(m_idum);
//
//		for (j = NTAB + 7; j >= 0; j--) {
//			k = (m_idum) / IQ;
//			m_idum = IA * (m_idum - k * IQ) - IR * k;
//			if (m_idum < 0) m_idum += IM;
//			if (j < NTAB) m_iv[j] = m_idum;
//		}
//		m_iy = m_iv[0];
//	}
//	k = (m_idum) / IQ;
//	m_idum = IA * (m_idum - k * IQ) - IR * k;
//	if (m_idum < 0) m_idum += IM;
//	j = m_iy / NDIV;
//	m_iy = m_iv[j];
//	m_iv[j] = m_idum;
//
//	return m_iy;
//}
//
//float C_Random::RandomFloat(float flLow, float flHigh)
//{
//	float fl = AM * (float)this->GenerateRandomNumber();
//	if (fl > RNMX) fl = RNMX;
//	return (fl * (flHigh - flLow)) + flLow;
//}
//
//void CNoSpread::CalcClient(Vector vSpreadVec, Vector ViewIn, Vector &ViewOut)
//{
//	Vector vecForward, vecRight, vecUp, vecSpreadDir;
//
//	Math::AngleVectors(ViewIn, &vecForward, &vecRight, &vecUp);
//
//	vecSpreadDir = vecForward + vecRight * -vSpreadVec.x + vecUp * -vSpreadVec.y;
//
//	Utils::VectorAngles(vecSpreadDir, ViewOut);
//
//}
//
//void CNoSpread::CalcServer(Vector vSpreadVec, Vector ViewIn, Vector &vecSpreadDir)
//{
//	Vector vecViewForward, vecViewRight, vecViewUp;
//
//	Math::AngleVectors(ViewIn, &vecViewForward, &vecViewRight, &vecViewUp);
//
//	vecSpreadDir = vecViewForward + vecViewRight * vSpreadVec.x + vecViewUp * vSpreadVec.y;
//}
//
//void CNoSpread::GetSpreadVec(CUserCmd*pCmd, Vector &vSpreadVec)
//{
//	C_CSPlayer *self = C_CSPlayer::GetLocalPlayer();
//
//	if (!self)
//		return;
//
//	CBaseCombatWeapon* localWeapon = self->GetActiveWeapon();
//
//	if (!localWeapon)
//		return;
//
//	Vector vSpread;
//
//	localWeapon->UpdateAccuracyPenalty();
//
//	Random->SetSeed((pCmd->random_seed & 0xFF) + 1);
//
//	float fRand1 = Random->RandomFloat(0.f, 1.f);
//	float fRandPi1 = Random->RandomFloat(0.f, 2.f * (float)M_PI);
//	float fRand2 = Random->RandomFloat(0.f, 1.f);
//	float fRandPi2 = Random->RandomFloat(0.f, 2.f * (float)M_PI);
//
//	if (localWeapon->GetId() == 64 && pCmd->buttons & IN_ATTACK2)
//	{
//		fRand1 = 1.f - fRand1 * fRand1;
//		fRand2 = 1.f - fRand2 * fRand2;
//	}
//
//	float fRandInaccuracy = fRand1 * localWeapon->GetInaccuracy();
//	float fRandSpread = fRand2 * localWeapon->GetSpread();
//
//	vSpreadVec.x = cos(fRandPi1) * fRandInaccuracy + cos(fRandPi2) * fRandSpread;
//	vSpreadVec.y = sin(fRandPi1) * fRandInaccuracy + sin(fRandPi2) * fRandSpread;
//
//}
//long double VectorNormalize2(float *flAngles)
//{
//	long double ldRet = sqrt(flAngles[0] * flAngles[0] + flAngles[1] * flAngles[1] + flAngles[2] * flAngles[2]);
//
//	if (ldRet != 0.0f)
//	{
//		flAngles[0] = flAngles[0] * (1.0f / (float)ldRet);
//		flAngles[1] = flAngles[1] * (1.0f / (float)ldRet);
//		flAngles[2] = flAngles[2] * (1.0f / (float)ldRet);
//	}
//
//	return ldRet;
//}
//void CNoSpread::CompensateInAccuracyNumeric(CUserCmd*cmd)
//{
//	Vector Aim, qAntiSpread;
//
//	Vector vSpreadVec, vForward, vServerSpreadForward;
//
//	Aim = cmd->viewangles;
//
//	Math::AngleVectors(Aim, &vForward);
//
//	GetSpreadVec(cmd, vSpreadVec);
//
//	CalcClient(vSpreadVec, Aim, qAntiSpread);
//
//	for (int i = 0; i <= 24; i++)
//	{
//		CalcServer(vSpreadVec, qAntiSpread, vServerSpreadForward);
//
//		Vector qModifer;
//		Utils::VectorAngles(vServerSpreadForward, qModifer);
//
//		qModifer = Aim - qModifer;
//
//		if (sqrt((qModifer.x * qModifer.x) + (qModifer.y * qModifer.y)) == 0)
//		{
//			break;
//		}
//
//		qAntiSpread = qAntiSpread + qModifer;
//
//		cmd->viewangles = qAntiSpread;
//	}
//
//	cmd->viewangles -= PredictPunchAngles();
//}
//#define square( x ) ( x * x ) 
//
//void VectorAngles(const Vector &forward, const Vector &pseudoup, Vector &angles)
//{
//	Vector left;
//
//	VectorCrossProduct(pseudoup, forward, left);
//
//	left.NormalizeInPlace();
//
//	float xyDist = sseSqrt(forward.x * forward.x + forward.y * forward.y);
//
//	if (xyDist > 0.001f)
//	{
//		angles[1] = RAD2DEG(atan2f(forward[1], forward[0]));
//		angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));
//
//		float up_z = (left[1] * forward[0]) - (left[0] * forward[1]);
//
//		angles[2] = RAD2DEG(atan2f(left[2], up_z));
//	}
//	else
//	{
//		angles[1] = RAD2DEG(atan2f(-left[0], left[1]));
//		angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));
//		angles[2] = 0;
//	}
//}
//
//void CNoSpread::RollSpread(CBaseCombatWeapon* localWeap, int seed, CUserCmd*cmd, Vector& pflInAngles)
//{
//	//pflInAngles.x = AngleNormalize(pflInAngles.x);
//	//pflInAngles.y = AngleNormalize(pflInAngles.y);
//
//	localWeap->UpdateAccuracyPenalty();
//
//	Random->SetSeed((seed & 0xFF) + 1);
//
//	cmd->random_seed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;
//
//	float fRand1 = Random->RandomFloat(0.f, 1.f);
//	float fRandPi1 = Random->RandomFloat(0.f, 2.f * (float)M_PI);
//	float fRand2 = Random->RandomFloat(0.f, 1.f);
//	float fRandPi2 = Random->RandomFloat(0.f, 2.f * (float)M_PI);
//
//	if (localWeap->GetId() == 64 && cmd->buttons & IN_ATTACK2)
//	{
//		fRand1 = 1.f - fRand1 * fRand1;
//		fRand2 = 1.f - fRand2 * fRand2;
//	}
//
//	float fRandInaccuracy = fRand1 * localWeap->GetInaccuracy();
//	float fRandSpread = fRand2 * localWeap->GetSpread();
//
//	Vector vForward, vRight, vUp, vDir;
//	Vector vView, vSpread, flIdentity[3];
//	float flRoll, flCross;
//
//	vSpread.x = cos(fRandPi1) * fRandInaccuracy + cos(fRandPi2) * fRandSpread;
//	vSpread.y = sin(fRandPi1) * fRandInaccuracy + sin(fRandPi2) * fRandSpread;
//
//	Vector qViewAngles = pflInAngles;
//
//	vSpread[0] = -vSpread[0];
//	vSpread[1] = -vSpread[1];
//
//	Math::AngleVectors(qViewAngles, &vForward, &vRight, &vUp);
//
//	vDir[0] = vForward[0] + (vRight[0] * vSpread[0]) + (vUp[0] * vSpread[1]);
//	vDir[1] = vForward[1] + (vRight[1] * vSpread[0]) + (vUp[1] * vSpread[1]);
//	vDir[2] = vForward[2] + (vRight[2] * vSpread[0]) + (vUp[2] * vSpread[1]);
//
//	vDir.NormalizeInPlace();
//
//	flIdentity[2][0] = 1.0f;
//	flIdentity[2][1] = -vSpread[0];
//	flIdentity[2][2] = vSpread[1];
//
//	flIdentity[2].NormalizeInPlace();
//
//	flIdentity[0][0] = 0.0f;
//	flIdentity[0][1] = -vSpread[0];
//	flIdentity[0][2] = (1.0f / vSpread[1]) + (1.0f / flIdentity[2][2]) + vSpread[1];
//
//	if (vSpread[0] > 0.0f && vSpread[1] < 0.0f)
//	{
//		if (flIdentity[0][1] < 0.0f)
//			flIdentity[0][1] = -flIdentity[0][1];
//	}
//	else if (vSpread[0] < 0.0f && vSpread[1] < 0.0f)
//	{
//		if (flIdentity[0][1] > 0.0f)
//			flIdentity[0][1] = -flIdentity[0][1];
//	}
//
//	if (flIdentity[0][2] < 0.0f)
//		flIdentity[0][2] = -flIdentity[0][2];
//
//	flIdentity[0].NormalizeInPlace();
//
//	VectorCrossProduct(flIdentity[0], flIdentity[2], flIdentity[1]);
//
//	flIdentity[1].NormalizeInPlace();
//
//	flCross = (flIdentity[1][1] * flIdentity[2][0]) - (flIdentity[1][0] * flIdentity[2][1]);
//
//	if (qViewAngles[0] > 84.0f || qViewAngles[0] < -84.0f)
//		flRoll = RAD2DEG(atan2f(flIdentity[1][2], sseSqrt(flCross)));
//	else
//		flRoll = RAD2DEG(atan2f(flIdentity[1][2], flCross));
//
//	if (flRoll < 0.0f)
//		flRoll += 360.0f;
//
//	VectorAngles(vDir, vUp, pflInAngles);
//
//	//pflInAngles.NormalizeAngle();
//	//pflInAngles.ClampAngle();
//
//	pflInAngles[2] += flRoll;
//
//	//pflInAngles.x = AngleNormalize(pflInAngles.x);
//	//pflInAngles.y = AngleNormalize(pflInAngles.y);
//}
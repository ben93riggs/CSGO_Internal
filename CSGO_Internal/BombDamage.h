#pragma once

class BombDamage
{
	static float CSGO_Armor(float flDamage, int ArmorValue)
	{
		float flArmorRatio = 0.5f;
		float flArmorBonus = 0.5f;
		if (ArmorValue > 0) {
			float flNew = flDamage * flArmorRatio;
			float flArmor = (flDamage - flNew) * flArmorBonus;

			if (flArmor > static_cast<float>(ArmorValue)) {
				flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
				flNew = flDamage - flArmor;
			}

			flDamage = flNew;
		}
		return flDamage;
	}

public:
	static int GetBombDamage(C_CSPlayer * pMe, C_CSPlayer * pC4)
	{
		float flDistance = pMe->GetOrigin().DistTo(pC4->GetOrigin());

		float a = 450.7f;
		float b = 75.68f;
		float c = 789.2f;
		float d = (flDistance - b) / c;
		float flDamage = a*exp(-d * d);

		return max((int)ceilf(CSGO_Armor(flDamage, pMe->GetArmorValue())), 0);
	}

};

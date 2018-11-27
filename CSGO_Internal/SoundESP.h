#pragma once
#include "Singleton.h"
#include "SourceEngine/Vector.hpp"

class SoundESP : public singleton<SoundESP>
{
	uint8_t border[4] = { 255, 0, 0, 255 };
	uint8_t filler[4] = { 0, 0, 255, 0 };
public:
	void EmitSound(int iEntIndex, const Vector* pOrigin) const;
};

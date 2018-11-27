#pragma once
#include "Singleton.h"

class RankRevealer : public singleton<RankRevealer>
{
public:
	void CreateMove() const;
};

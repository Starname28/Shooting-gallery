#pragma once
#include "Target.h"

#include <vector>

class TargetManager
{
public:
	TargetManager(int count, float minSizeTarget, float maxSizeTarget);

	void Init();
	void Draw();
	void Update(float dt);

	bool IsHit(FPoint posArrow, EffectsContainer& eff);
	void Clear();
	int GetCurrentCountTarget();

private:
	std::vector <Target*> targets;

	int count;
	float minSize, maxSize;
};


#pragma once
#include "Target.h"

#include <vector>

class TargetManager
{
public:
	TargetManager(int count, float minSizeTarget, float maxSizeTarget);
	~TargetManager();

	void Init();
	void Draw();
	void Update(float dt);

	bool IsHit(FPoint cannonballPos, EffectsContainer& eff);
	void Clear();
	int GetCountTarget();
	void SetCount(size_t count);

private:
	std::vector <Target*> targets;

	int count;
	float minSize, maxSize;
};


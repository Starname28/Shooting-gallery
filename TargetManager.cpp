#include "stdafx.h"
#include "TargetManager.h"

TargetManager::TargetManager(int count, float minSizeTarget, float maxSizeTarget)
	: count(count)
	, minSize(minSizeTarget)
	, maxSize(maxSizeTarget)
{
	Init();
}

void TargetManager::Init()
{
	for (int i = 0; i < count; i++)
		targets.push_back(
			new Target(
				FPoint(math::random(150, 1024 - 150), math::random(200, 768 - 200)),
				minSize, maxSize
			));
}

void TargetManager::Draw()
{
	for (auto target : targets)
		target->Draw();
}

void TargetManager::Update(float dt)
{
	for (auto target : targets)
		target->Update(dt);
}

bool TargetManager::IsHit(FPoint cannonballPos, EffectsContainer& eff)
{
	for (int i = targets.size() - 1; i >= 0; i--)
		if (!targets[i]->GetHit() && targets[i]->IsHit(cannonballPos, eff))
		{
			delete targets[i];
			targets.erase(targets.begin() + i);
			targets.size();

			return true;
		}

	return false;
}

void TargetManager::Clear()
{
	for (auto target : targets)
		delete target;

	targets.clear();
}

int TargetManager::GetCountTarget()
{
	return targets.size();
}

void TargetManager::SetCount(size_t count)
{
	this->count = count;
}


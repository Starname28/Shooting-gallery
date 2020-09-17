#pragma once
#include "Cannonball.h"
#include "TargetManager.h"
#include <vector>

class Cannon
{
public:
	Cannon();
	
	void Draw();
	void Update(float dt);

	bool MouseDown(const IPoint& pos, int speed);

	int countCannonball() const;
	void Clear();
	void DeleteCannonBall(size_t n);

	void isHit(TargetManager* targets, EffectsContainer& eff);
private:

	void Init();

	Render::Texture* cannon;
	IPoint position;

	float scale;
	float angle;
	
	std::vector<Cannonball*> cannonballs;
};


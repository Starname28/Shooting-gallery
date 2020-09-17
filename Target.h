#pragma once
#include <PlayrixEngine.h>
#include <string>

class Target
{
public:
	Target(FPoint position, float minSize, float maxSize);
	~Target();

	void Draw();
	void Update(float dt);

	FPoint GetPosition() const;

	bool IsHit(FPoint cannonballPos, EffectsContainer &effCont);
	bool GetHit() const;

private:
	bool WasHit(FPoint cannonballPos);
	float random(float min, float max);
	std::string randomTex();


	Render::Texture* target;

	FPoint position;
	FPoint speedVec;

	float speed;
	float scale;

	bool is_hit;
};


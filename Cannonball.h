#pragma once
#include <PlayrixEngine.h>

enum {
	WINDOW_WIDTH = 1024,
	WINDOW_HEIGHT = 768
};

class Cannonball
{
public:
	Cannonball(float scale, float speed, float angle);
	~Cannonball();

	void Draw();
	void Update(float dt);

	void SetHit(bool hit);
	bool GetHit() const;

	FPoint GetPosition();
private:

	void Init();

	Render::Texture* cannonball;
	FPoint position;

	float scale;
	float speed;
	float angle;

	bool isHit;
};


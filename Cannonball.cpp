#include "stdafx.h"
#include "Cannonball.h"

Cannonball::Cannonball(float scale, float speed, float angle)
	: scale(scale),
	  speed(speed),
	  angle(angle)
{
	this->Init();
}

Cannonball::~Cannonball()
{
}

void Cannonball::Draw()
{
	if (!isHit) 
	{
		Render::device.PushMatrix();
		Render::device.MatrixTranslate(math::Vector3(position.x, position.y, 0));
		Render::device.MatrixRotate(math::Vector3(0, 0, 1), angle);

		IRect texRect = cannonball->getBitmapRect();
		FRect rect(texRect), uv(0, 1, 0, 1);
		cannonball->TranslateUV(rect, uv);

		Render::device.MatrixScale(scale);

		Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.f, -texRect.height, 0.f));

		cannonball->Bind();
		Render::DrawQuad(rect, uv);

		Render::device.PopMatrix();
	}
}

void Cannonball::Update(float dt)
{
	position.x += speed * dt * 10 * math::cos((angle + 90) * math::PI / 180);
	position.y += speed * dt * 10* math::sin((angle + 90) * math::PI / 180);
}

void Cannonball::SetHit(bool hit)
{
	isHit = hit;
}

bool Cannonball::GetHit() const
{
	return isHit;
}

FPoint Cannonball::GetPosition()
{
	return position;
}

void Cannonball::Init()
{
	cannonball = Core::resourceManager.Get<Render::Texture>("Cannonball");
	position = IPoint(WINDOW_WIDTH / 2, 100);
	isHit = false;
}

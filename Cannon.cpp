#include "stdafx.h"
#include "Cannon.h"

Cannon::Cannon()
	: scale(0.08f),
	  angle(0)
{
	Init();
}

void Cannon::Draw()
{
	for (const auto& cannonball : cannonballs)
		cannonball->Draw();

	Render::device.PushMatrix();

	Render::device.MatrixTranslate(math::Vector3(position.x, position.y, 0));
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), angle);

	IRect texRect = cannon->getBitmapRect();
	FRect rect(texRect), uv(0, 1, 0, 1);
	cannon->TranslateUV(rect, uv);

	Render::device.MatrixScale(scale);
	Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.0f, -texRect.height / 2.0f, 0.0f));

	cannon->Bind();

	Render::DrawQuad(rect, uv);

	Render::device.PopMatrix();
}

void Cannon::Update(float dt)
{
	size_t i = 0;

	for(auto cannonball : cannonballs)
	{
		cannonball->Update(dt);

		FPoint aPoint = cannonball->GetPosition();

		if ((aPoint.y > (WINDOW_HEIGHT + 20)) ||
			(aPoint.y < (-20)) ||
			(aPoint.x > (WINDOW_WIDTH + 20)) ||
			(aPoint.x < (-20))) {

			DeleteCannonBall(i);
		}
		else 
		{
			i++;
		}
	}
	IPoint mouse_pos = Core::mainInput.GetMousePos();

	if ((mouse_pos.x - position.x) != 0) 
	{
			angle = math::atan((mouse_pos.y - position.y), (mouse_pos.x - position.x)) * 180.0f / math::PI - 90;
	}
}

bool Cannon::MouseDown(const IPoint& pos, int speed)
{
	cannonballs.push_back(new Cannonball(scale, speed, angle));

	return false;
}

int Cannon::countCannonball() const
{
	return cannonballs.size();
}

void Cannon::Clear()
{
	for (auto cannonball : cannonballs)
	{
		delete cannonball;
	}

	cannonballs.clear();
}

void Cannon::DeleteCannonBall(size_t n)
{
	delete cannonballs[n];
	cannonballs.erase(cannonballs.begin() + n);
}

void Cannon::isHit(TargetManager * targets, EffectsContainer& eff)
{
	for (int i = cannonballs.size() - 1; i >= 0; i--)
		if (!cannonballs[i]->GetHit())
			if (targets->IsHit(cannonballs[i]->GetPosition(), eff))
				cannonballs[i]->SetHit(true);
}

void Cannon::Init()
{
	cannon = Core::resourceManager.Get<Render::Texture>("Cannon");
	position = IPoint(WINDOW_WIDTH / 2, 100);
}

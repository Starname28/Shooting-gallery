#include "stdafx.h"
#include "Target.h"

Target::Target(FPoint position, float minSize, float maxSize)
	: position(position)
	, speed(random(50.0f, 800.f))
	, scale(random(minSize, maxSize))
	, is_hit(false)
	, target(Core::resourceManager.Get<Render::Texture>(randomTex()))
{
	dSpeed.x = random(-1.0f, 1.0f);
	dSpeed.y = sqrt(1 - dSpeed.x * dSpeed.x);

	dSpeed.x *= speed;
	dSpeed.y *= speed;
}

Target::~Target()
{
}

void Target::Draw()
{
	if (!is_hit)
	{
		Render::device.PushMatrix();

		Render::device.MatrixTranslate(math::Vector3(position.x, position.y, 0));

		IRect texRect = target->getBitmapRect();
		FRect rect(texRect), uv(0, 1, 0, 1);
		target->TranslateUV(rect, uv);

		Render::device.MatrixScale(scale);
		Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.f, -texRect.height / 2.f, 0.f));

		target->Bind();
		Render::DrawQuad(rect, uv);
		Render::device.PopMatrix();
	}
}

void Target::Update(float dt)
{
	IRect texRect = target->getBitmapRect();

	if ((position.x + dSpeed.x * dt) > (1024 - texRect.width * scale) ||
		((position.x + dSpeed.x * dt) < (texRect.width * scale)))
	{
		dSpeed.x = -dSpeed.x;
	}

	if ((position.y + dSpeed.y * dt) > (768 - texRect.height * scale) ||
		((position.y + dSpeed.y * dt) < (texRect.height * scale + 100)))
	{
		dSpeed.y = -dSpeed.y;
	}

	position.x += dSpeed.x * dt;
	position.y += dSpeed.y * dt;

	if (is_hit) {
		scale = 0;
	}
}

FPoint Target::GetPosition() const
{
	return position;
}

bool Target::IsHit(FPoint cannonballPos, EffectsContainer &effCont)
{
	if (WasHit(cannonballPos))
	{
		is_hit = true;

		ParticleEffectPtr eff = effCont.AddEffect("BoomBalloons");
		eff->posX = position.x + 0.f;
		eff->posY = position.y + 0.f;
		eff->Reset();
		eff->Finish();
	}

	return is_hit;
}

bool Target::GetHit() const
{
	return is_hit;
}

bool Target::WasHit(FPoint cannonballPos)
{
	IRect texRect = target->getBitmapRect();

	return ((cannonballPos.x) >= (position.x - texRect.width / 2 * scale)) &&
		   ((cannonballPos.x) <= (position.x - texRect.width / 2 * scale + texRect.width * scale)) &&
		   ((cannonballPos.y) >= (position.y - texRect.height / 2 * scale)) &&
		   ((cannonballPos.y) <= (position.y - texRect.height / 2 * scale + texRect.height * scale));
}

float Target::random(float min, float max)
{
	return math::random(min, max);;
}

std::string Target::randomTex()
{
	int targetTexture = math::random(0, 5);

	switch (targetTexture)
	{
	case 0: return "Target";
	case 1: return "Target1";
	case 2: return "Target2";
	case 3: return "Target3";
	case 4: return "Target4";
	case 5: return "Target5";
	}

	return "";
}

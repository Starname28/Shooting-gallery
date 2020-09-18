#pragma once
#include "Cannon.h"
#include "ParseAndStore.h"
#include "Timer.h"

#include <memory>

enum GameState { START = 0, GAME, LOSE, WIN };

class GameWidget : public GUI::Widget
{
public:
	GameWidget(const std::string& name, rapidxml::xml_node<>* elem);
	~GameWidget();

	void Draw() override;
	void Update(float dt) override;

	void AcceptMessage(const Message& message) override;

	bool MouseDown(const IPoint& mouse_pos) override;
	void MouseMove(const IPoint& mouse_pos) override;
	void MouseUp(const IPoint& mouse_pos) override;

	void KeyPressed(int keyCode) override;
	void CharPressed(int unicodeChar) override;

private:
	void Init();

private:
	void EndDisplay(GameState gameState, const std::string& font, const std::string& inscription);
	void ScaleText();
	void DefaultSettingsText();
	void PrintText(const std::string& font, float posX, float posY, const std::string& text, float scale);
	void DrawScaleText(Render::Texture* tex, float posX, float posY, float scale);
	void Restart();

	float _timer;

	float _scale;
	float _angle;

	float scaleText;
	float scaleTextBelow ;
	float stepScaleText;
	bool isLess;

	std::unique_ptr<TargetManager> targets;
	std::unique_ptr<Cannon> cannon;

	Render::Texture* background;
	Render::Texture* aim;
	Render::Texture* clock;
	Render::Texture* trophy;
	int _curTex;
	int level;
	int targetsCount;

	EffectsContainer _effCont;
	ParticleEffectPtr _eff;

	ParseAndStore parser;
	Timer timer;

	GameState state;

	bool firstCall;
	bool countTargetMoreZero;
};
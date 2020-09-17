#include "stdafx.h"
#include "GameObject.h"

GameWidget::GameWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, targets(nullptr)
	, cannon(nullptr)
	, _curTex(0)
	, _timer(0)
	, _angle(0)
	, scaleText(0.01)
	, scaleTextBelow(0.01)
	, stepScaleText(0.005)
	, isLess(true)
	, _eff(NULL)
	, _scale(0.f)
	, state(GameState::START)
	, firstCall(true)
{
	parser.Parse();

	background = Core::resourceManager.Get<Render::Texture>("Background");
	targets = std::make_unique<TargetManager>(static_cast<int>(parser.countTarget), parser.minTargetSize, parser.maxTargetSize);
}

GameWidget::~GameWidget()
{
	cannon->Clear();
	targets->Clear();
}

void GameWidget::Init()
{
	cannon = std::make_unique<Cannon>();

	timer.SetEndTime(parser.time);
	timer.Start();

	_tex5 = Core::resourceManager.Get<Render::Texture>("Aim");
	clock = Core::resourceManager.Get<Render::Texture>("Clock");
	_curTex = 0;
	_angle = 0;
}

void GameWidget::EndDisplay(GameState gameState, const std::string& font, const std::string& inscription)
{
	state = gameState;
	const int countTarget = parser.countTarget;
	ScaleText();

	if (!Render::isFontLoaded(font))
		return;

	Render::BindFont(font);
	Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 3 * 2, std::string(inscription), scaleText, CenterAlign);
	Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 3 * 1.5, "Press  for restart", scaleTextBelow, CenterAlign);

	Render::PrintString(
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 3 * 1.3, 
		"Score: " + utils::lexical_cast(countTarget - targets->GetCurrentCountTarget()) + "/" + utils::lexical_cast(countTarget), 
		scaleText/2, CenterAlign);

	cannon->Clear();

	if (Core::mainInput.GetMouseRightButton())
	{
		state = GameState::GAME;
		targets->Clear();
		timer.Start();
		targets->Init();
	}
}

void GameWidget::ScaleText()
{
	if (scaleText < 1.f)
		scaleText += 2 * stepScaleText;

	if (scaleTextBelow < 0.5f && isLess)
	{
		scaleTextBelow += stepScaleText;
	}
	else
	{
		isLess = false;
		stepScaleText = 0.001;

		scaleTextBelow -= stepScaleText;

		if (scaleTextBelow < 0.4f)
			isLess = true;
	}
}

void GameWidget::DefaultSettingsText()
{
	scaleText = 0.01;
	scaleTextBelow = 0.01;
	isLess = true;
	stepScaleText = 0.01;
}

void GameWidget::PrintText(const std::string & font, float posX, float posY, const std::string & text, float scale)
{
	if (!Render::isFontLoaded(font))
		return;

	Render::BindFont(font);
	Render::PrintString(posX, posY, text, scale, CenterAlign);
}

void GameWidget::Draw()
{
	background->Draw();

	if (state != GameState::START)
	{
		//
		// �������� ������� ��������� ������� ����.
		//

		IPoint mouse_pos = Core::mainInput.GetMousePos();

		//
		// ������������ � ���� ������� �������������� ���������, ����� � ����������
		// ����� ���� ������������ ��� �������������� ������� PopMatrix.
		//
		Render::device.PushMatrix();

		//
		// �������� ������� �������������� ���������, ��������� ����� ��������� � ������� ����
		// � ����������� ���������� ������������ ����� ������ ������ ��� z �� ���� _angle.
		//
		Render::device.MatrixTranslate((float)mouse_pos.x, (float)mouse_pos.y, 0);
		Render::device.MatrixRotate(math::Vector3(0, 0, 1), _angle);

		// ����� Texture::Draw() ������� ���������� ������ � ��������� ��������
		// � ������ ��������� (0, 0). ����� ��������� � �������������� ��������� ������
		// ������� ��������������� � ����������� �������� ��������������.
		//
		// ��� ������ ������ Texture::Draw() �������� Texture::Bind() �������������.
		//
		IRect texRect = _tex5->getBitmapRect();

		//
		// ��� ��������� �������� ����� ������� �������� UV ���������� ��� ����� ��������,
		// ������� ����� �������� �� ������� �������. UV ���������� ������ ���� �������������,
		// �.�., ��� ����������� �� ������� �������� � ��������, ������ ����� ��������
		// ����� 1 (UV ���������� �������� � ��������� 0..1, ���� ����� �� ������ ������ ��
		// ������ �������, ��� ���� � ������ ������������� ��������� �������� REPEAT, ��������
		// ����� ���������� �� ���� ������� ��������������� ���������� ���).
		//

		FRect rect(texRect);
		FRect uv(0, 1, 0, 1);

		_tex5->TranslateUV(rect, uv);

		Render::device.MatrixScale(0.7 * _scale);
		Render::device.MatrixTranslate(-texRect.width * 0.5f, -texRect.height * 0.5f, 0.0f);

		//
		// ����������� ��������.
		//
		_tex5->Bind();

		//
		// ����� DrawQuad() ������� � ����������� ���������� ���������� ������, ��������� �� ����
		// ���������� - �������������, ��������� ��� ���� ������� ���� ��� ������ � ����������� (binded) ��������,
		// ���� ��������� ���������������.
		//
		// ����� ������� DrawQuad() ������ ���� ����� Texture::Bind() ���� SetTexturing(false),
		// ����� ���������� ��������� ����� ���������������.
		//
		Render::DrawQuad(rect, uv);
		
		// ��������������� ������� �������������� ���������, ������ �� ����� ��������� �����.
		Render::device.PopMatrix();

		//
		// ���� ����� ��������� ��������������� ��� ���������.
		//
		Render::device.SetTexturing(false);

		//
		// ����� BeginColor() ������������ � ���� ������� ���� ������ � ������������� �����.
		//
		Render::BeginColor(Color(255, 128, 0, 255));

		//
		// ����� DrawRect() ������� � ����������� ���������� ���������� ������, ��������� �� ����
		// ���������� - �������������, ��������� ��� ���� ������� ���� ��� ������ � ����������� (binded) ��������,
		// ���� ��������� ���������������.
		//
		// ����� ������� DrawRect() ������ ���� ����� Texture::Bind(), ���� SetTexturing(false),
		// ����� ���������� ��������� ����� ���������������.
		//
		Render::DrawRect(924, 0, 100, 100);

		//
		// ����� EndColor() ������� �� ����� ������� ���� ������, �������������� �������.
		//
		Render::EndColor();

		//
		// ����� �������� ���������������.
		//
		Render::device.SetTexturing(true);

		//
		// ������ ��� �������, ������� �������� � ��������� (Update() ��� ���������� �������� �� �����).
		//
		_effCont.Draw();
		targets->Draw();
		cannon->Draw();

		PrintText("arial", 924 + 100 / 2, 35, utils::lexical_cast(mouse_pos.x) + ", " + utils::lexical_cast(mouse_pos.y), 1.f);

		if (!timer.IsTimeEnd() && targets->GetCurrentCountTarget() != 0)
		{

			Render::device.PushMatrix();

			Render::device.MatrixTranslate(math::Vector3(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT - 40, 0));
			Render::device.MatrixRotate(math::Vector3(0, 0, 1), 0);

			IRect texRect = clock->getBitmapRect();
			FRect rect(texRect), uv(0, 1, 0, 1);

			Render::device.MatrixScale(0.3);
			Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.0f, -texRect.height / 2.0f, 0.0f));

			clock->Bind();

			Render::DrawQuad(rect, uv);

			Render::device.PopMatrix();

			PrintText("lemon_regular", WINDOW_WIDTH / 2+30, WINDOW_HEIGHT - 10, 
				utils::lexical_cast(timer.TimeNow()) + std::string("/") + utils::lexical_cast(timer.GetEndTime()), 1.0f);

			Render::BindFont("lemon_regular");
			Render::PrintString(
				WINDOW_WIDTH / 6 - 150, WINDOW_HEIGHT - 10,
				std::string("Target count: ") + utils::lexical_cast(targets->GetCurrentCountTarget()),
				1.f, LeftAlign);
		}
		else if (targets->GetCurrentCountTarget() == 0)
		{
			EndDisplay(GameState::WIN, "emon", "!!!You win!!!");
		}
		else
		{
			EndDisplay(GameState::LOSE, "red_emon", "!!!You lose!!!");
		}
	}
	else
	{
	    targets->Draw();
		ScaleText();

		float posX = WINDOW_WIDTH / 2;
		float posY = WINDOW_HEIGHT / 3;

		PrintText("starborn", posX, posY * 1.85, "Shooting gallery", scaleText);
		PrintText("starborn", posX, posY * 1.55, "with balloons", scaleText * 0.7);

		if (targets->GetCurrentCountTarget() != 0)
		{
			PrintText("min_starborn", posX, posY * 1.2, "Press RMB for start game", scaleTextBelow);
			countTargetMoreZero = true;
		}
		else
		{
			PrintText("min_starborn", posX, posY * 1.2, "Please enter the number of target greater than 0", scaleTextBelow);
			countTargetMoreZero = false;
		}
	}
}

void GameWidget::Update(float dt)
{
	if (state != GameState::START)
	{
		//
		// ������� ��������� � ���������
		//
		targets->Update(dt);

		cannon->Update(dt);

		cannon->isHit(targets.get(), _effCont);

		_effCont.Update(dt);

		//
		// dt - �������� ������� � ��������, ��������� �� ����������� �����.
		// ��� ����� ��������� ������ ��������, � ����������� �� ������������������ �������
		// � ��������� �����.
		//
		// ��� ����, ����� ���� �������� �������� ������ �� �������, � �� �������� ��
		// ������������������ �������, �� ������ ������������ � �� ����� ��������.
		//
		// ����������� ��� ������ � ��������� ���������.
		//
		_timer += dt * 2;

		//
		// ����������� ������ � ��������� (0, 2�).
		// ��� ����� ������ ��� �������������� ��������� ������������ ��������,
		// ���� ����� ������������ ��������� ����� (float ������������) ��� ��������
		// �� ����������� ����� ���� ������� ������� (������� �������-�-��).
		//
		// �������� �������� ������ ������ (0, 2�), ������ ��� �� ���������� ��� ��������
		// ��� ������� ������, � ������� �������� ����� ������ ������������ ��������� ���������.
		//
		while (_timer > 2 * math::PI)
		{
			_timer -= 2 * math::PI;
		}

		//
		// ������������ ��������� ��������������� � ����������� �� �������.
		//
		_scale = 0.8f + 0.08f * sinf(_timer);
	}
}

bool GameWidget::MouseDown(const IPoint& mouse_pos)
{
	if (Core::mainInput.GetMouseRightButton())
	{
		//
		// ��� ������� �� ������ ������ ����, ������ ������ ������ �� ������.
		//
		_eff = _effCont.AddEffect("Iskra");
		_eff->posX = mouse_pos.x + 0.f;
		_eff->posY = mouse_pos.y + 0.f;
		_eff->Reset();

		//
		// � �������� ���� ������� ��������.
		//
	/*	_angle += 30;
		while (_angle > 360)
		{
			_angle -= 360;
		}*/
		DefaultSettingsText();

		if(countTargetMoreZero)
			state = GameState::GAME;
		
		if (firstCall)
		{
			firstCall = false;
			Init();
		}
	}
	else
	{
		//
		// ��� ������� �� ����� ������ ����, ������ ��������� ������, ������� ���������� ���.
		//
		if (state == GameState::GAME)
			cannon->MouseDown(mouse_pos, parser.cannonballSpeed);
		else if (state == GameState::WIN || state == GameState::LOSE)
		{
			ParticleEffectPtr eff = _effCont.AddEffect("FindItem2");
			eff->posX = mouse_pos.x + 0.f;
			eff->posY = mouse_pos.y + 0.f;
			eff->Reset();
		}
		// �������� �������� � 0 �� 1 � ��������.
		//
		_curTex = 1 - _curTex;
	}
	return false;
}

void GameWidget::MouseMove(const IPoint& mouse_pos)
{
	if (_eff)
	{
		//
		// ���� ������ ������, �� ���������� ��� � ������� ����.
		//
		_eff->posX = mouse_pos.x + 0.f;
		_eff->posY = mouse_pos.y + 0.f;
	}
}

void GameWidget::MouseUp(const IPoint& mouse_pos)
{
	if (_eff)
	{
		//
		// ���� ������ ������, �� ��� ���������� ���� ��������� ���.
		//
		_eff->Finish();
		_eff = NULL;
	}
}

void GameWidget::AcceptMessage(const Message& message)
{
	//
	// ������� ����� ���������� ��������� � �����������.
	//

	const std::string& publisher = message.getPublisher();
	const std::string& data = message.getData();
}

void GameWidget::KeyPressed(int keyCode)
{
	//
	// keyCode - ����������� ��� �������.
	// � �������� �������� ��� �������� ����� ������������ ��������� VK_.
	//

	if (keyCode == VK_A) {
		// ������� �� ������� ������ A
	}
}

void GameWidget::CharPressed(int unicodeChar)
{
	//
	// unicodeChar - Unicode ��� ��������� �������
	//

	if (unicodeChar == L'�') {
		// ������� �� ���� ������� '�'
	}
}

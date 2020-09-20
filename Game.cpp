#include "stdafx.h"
#include "Game.h"

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
	, level(1)
	, maxLevel(10)
	, targetsCount(0)
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
}

void GameWidget::Init()
{
	cannon = std::make_unique<Cannon>();

	timer.SetEndTime(parser.time);
	timer.Start();

	aim = Core::resourceManager.Get<Render::Texture>("Aim");
	clock = Core::resourceManager.Get<Render::Texture>("Clock");
	trophy = Core::resourceManager.Get<Render::Texture>("Trophy");

	targetsCount = static_cast<int>(parser.countTarget);
	level = static_cast<int>(parser.level);

	targets->Clear();
	targetsCount = parser.countTarget + 2 * level;
	targets->SetCount(targetsCount);
	timer.SetEndTime(parser.time - 2 * level);

	_curTex = 0;
	_angle = 0;
}

void GameWidget::EndDisplay(GameState gameState, const std::string& font, const std::string& inscription)
{
	state = gameState;
	ScaleText();

	if (state == GameState::GAMEOVER)
	{
		Render::BindFont(font);
		Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 4 * 2.5 - 30, "You have completed the game! Congratulations!!", scaleText*0.45, CenterAlign);
		Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 3 * 1.5 - 30, "Press Q for start new", scaleTextBelow*0.8, CenterAlign);
		return;
	}

	auto pred = [&gameState]() {
		return (gameState == GameState::WIN) ? "Press ENTER for start next level or" : "Press ENTER or";
	};

	if (!Render::isFontLoaded(font))
		return;

	Render::BindFont(font);
	Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 3 * 2, std::string(inscription), scaleText, CenterAlign);
	Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 3 * 1.5, std::string(pred()) + " SPACE for restart", scaleTextBelow*0.8, CenterAlign);
	Render::PrintString(
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 3 * 1.3, 
		"Score: " + utils::lexical_cast(targetsCount - targets->GetCountTarget()) + "/" + utils::lexical_cast(targetsCount),
		scaleText/2, CenterAlign);

	cannon->Clear();
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

void GameWidget::DrawScaleText(Render::Texture* tex, float posX, float posY, float scale)
{
	Render::device.PushMatrix();

	Render::device.MatrixTranslate(math::Vector3(posX, posY, 0));
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), 0);

	IRect texRect = tex->getBitmapRect();
	FRect rect(texRect), uv(0, 1, 0, 1);

	Render::device.MatrixScale(scale);
	Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.0f, -texRect.height / 2.0f, 0.0f));

	tex->Bind();

	Render::DrawQuad(rect, uv);

	Render::device.PopMatrix();
}

void GameWidget::Restart()
{
	DefaultSettingsText();

	state = GameState::GAME;
	targets->Clear();
	timer.Start();

	targets->Init();
}

void GameWidget::FirstCall()
{
	if (firstCall)
	{
		firstCall = false;
		Init();
	}
}

void GameWidget::Draw()
{
	background->Draw();

	if (state != GameState::START)
	{
		targets->Draw();
		//
		// Получаем текущее положение курсора мыши.
		//
		IPoint mouse_pos = Core::mainInput.GetMousePos();

		//
		// Проталкиваем в стек текущее преобразование координат, чтобы в дальнейшем
		// можно было восстановить это преобразование вызовом PopMatrix.
		//
		Render::device.PushMatrix();

		//
		// Изменяем текущее преобразование координат, перемещая центр координат в позицию мыши
		// и поворачивая координаты относительно этого центра вокруг оси z на угол _angle.
		//
		Render::device.MatrixTranslate((float)mouse_pos.x, (float)mouse_pos.y, 0);
		Render::device.MatrixRotate(math::Vector3(0, 0, 1), _angle);

		// Метод Texture::Draw() выводит квадратный спрайт с размерами текстуры
		// в центре координат (0, 0). Центр координат и преобразование координат вершин
		// спрайта устанавливаются с применением текущего преобразования.
		//
		// При вызове метода Texture::Draw() вызывать Texture::Bind() необязательно.
		//
		IRect texRect = aim->getBitmapRect();

		//
		// При отрисовке текстуры можно вручную задавать UV координаты той части текстуры,
		// которая будет натянута на вершины спрайта. UV координаты должны быть нормализованы,
		// т.е., вне зависимости от размера текстуры в текселях, размер любой текстуры
		// равен 1 (UV координаты задаются в диапазоне 0..1, хотя ничто не мешает задать их
		// больше единицы, при этом в случае установленной адресации текстуры REPEAT, текстура
		// будет размножена по этой стороне соответствующее количество раз).
		//

		FRect rect(texRect);
		FRect uv(0, 1, 0, 1);

		aim->TranslateUV(rect, uv);

		Render::device.MatrixScale(0.7 * _scale);
		Render::device.MatrixTranslate(-texRect.width * 0.5f, -texRect.height * 0.5f, 0.0f);

		//
		// Привязываем текстуру.
		//
		aim->Bind();

		//
		// Метод DrawQuad() выводит в графическое устройство квадратный спрайт, состоящий их двух
		// примитивов - треугольников, используя при этом текущий цвет для вершин и привязанную (binded) текстуру,
		// если разрешено текстурирование.
		//
		// Перед вызовом DrawQuad() должен быть вызов Texture::Bind() либо SetTexturing(false),
		// иначе визуальный результат будет непредсказуемым.
		//
		Render::DrawQuad(rect, uv);
		
		// Воостанавливаем прежнее преобразование координат, снимая со стека изменённый фрейм.
		Render::device.PopMatrix();

		//
		// Этот вызов отключает текстурирование при отрисовке.
		//
		Render::device.SetTexturing(false);

		//
		// Метод BeginColor() проталкивает в стек текущий цвет вершин и устанавливает новый.
		//
		Render::BeginColor(Color(255, 128, 0, 255));

		//
		// Метод DrawRect() выводит в графическое устройство квадратный спрайт, состоящий их двух
		// примитивов - треугольников, используя при этом текущий цвет для вершин и привязанную (binded) текстуру,
		// если разрешено текстурирование.
		//
		// Перед вызовом DrawRect() должен быть вызов Texture::Bind(), либо SetTexturing(false),
		// иначе визуальный результат будет непредсказуемым.
		//
		Render::DrawRect(924, 0, 100, 100);

		//
		// Метод EndColor() снимает со стека текущий цвет вершин, восстанавливая прежний.
		//
		Render::EndColor();

		//
		// Опять включаем текстурирование.
		//
		Render::device.SetTexturing(true);

		//
		// Рисуем все эффекты, которые добавили в контейнер (Update() для контейнера вызывать не нужно).
		//
		
		cannon->Draw();
		_effCont.Draw();

		PrintText("arial", 924 + 100 / 2, 35, utils::lexical_cast(mouse_pos.x) + ", " + utils::lexical_cast(mouse_pos.y), 1.f);

		if (!timer.IsTimeEnd() && targets->GetCountTarget() != 0)
		{
			DrawScaleText(clock, WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT - 40, 0.3);

			PrintText("lemon_regular", WINDOW_WIDTH / 2 + 30, WINDOW_HEIGHT - 10, 
				utils::lexical_cast(timer.TimeNow()) + std::string("/") + utils::lexical_cast(timer.GetEndTime()), 1.0f);

			Render::BindFont("lemon_regular");
			Render::PrintString(
				WINDOW_WIDTH / 6 - 150, WINDOW_HEIGHT - 10,
				std::string("Target count: ") + utils::lexical_cast(targets->GetCountTarget()),
				1.f, LeftAlign);
			Render::PrintString(
				WINDOW_WIDTH / 6 * 5, WINDOW_HEIGHT - 10,
				"Level: " + utils::lexical_cast(level) + "/" + utils::lexical_cast(maxLevel),
				1.f, CenterAlign);
		}
		else if (targets->GetCountTarget() == 0)
		{
			if (level == maxLevel)
			{
				state = GameState::GAMEOVER;
				DrawScaleText(trophy, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 4 * 3 - 30, 0.2 * scaleText);

				EndDisplay(GameState::GAMEOVER, "emon", "You win");
			}
			else
			{
				saver.Write(level + 1);

				EndDisplay(GameState::WIN, "emon", "You win");

				DrawScaleText(trophy, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 4 * 3, 0.1 * scaleText);

				Render::BindFont("lemon_regular");
				Render::PrintString(
					WINDOW_WIDTH / 2, WINDOW_HEIGHT / 4 * 3 - 30,
					"Level: " + utils::lexical_cast(level) + " completed",
					scaleText, CenterAlign
				);
			}
		}
		else
		{
			EndDisplay(GameState::LOSE, "red_emon", "Time out");
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

		if (targets->GetCountTarget() > 0)
		{
			PrintText("min_starborn", posX, posY * 1.2, "Press ENTER for continue game or Q for start new game", scaleTextBelow);
			countTargetMoreZero = true;
		}
		else
		{
			PrintText("min_starborn", posX, posY * 1.2, "Please enter the number of target greater than 0", scaleTextBelow);
			countTargetMoreZero = false;
		}
		PrintText("min_starborn", posX, posY * 1.1, "ESC to exit", scaleTextBelow);
	}
}

void GameWidget::Update(float dt)
{
	if (state != GameState::START)
	{
		//
		// Обновим контейнер с эффектами
		//
		targets->Update(dt);

		cannon->Update(dt);

		cannon->isHit(targets.get(), _effCont);

		_effCont.Update(dt);

		//
		// dt - значение времени в секундах, прошедшее от предыдущего кадра.
		// Оно может принимать разные значения, в зависимости от производительности системы
		// и сложности сцены.
		//
		// Для того, чтобы наша анимация зависела только от времени, и не зависела от
		// производительности системы, мы должны рассчитывать её от этого значения.
		//
		// Увеличиваем наш таймер с удвоенной скоростью.
		//
		_timer += dt * 2;

		//
		// Зацикливаем таймер в диапазоне (0, 2п).
		// Это нужно делать для предотвращения получения некорректных значений,
		// если вдруг переполнится разрядная сетка (float переполнился) или задержка
		// от предыдущего кадра была слишкой большой (система тормози-и-ит).
		//
		// Диапазон значений выбран равным (0, 2п), потому что мы используем это значение
		// для расчёта синуса, и большие значения будут просто периодически повторять результат.
		//
		while (_timer > 2 * math::PI)
		{
			_timer -= 2 * math::PI;
		}

		//
		// Анимирование параметра масштабирования в зависимости от таймера.
		//
		_scale = 0.8f + 0.08f * sinf(_timer);
	}
}

bool GameWidget::MouseDown(const IPoint& mouse_pos)
{
	if (Core::mainInput.GetMouseRightButton())
	{
		//
		// При нажатии на правую кнопку мыши, создаём эффект шлейфа за мышкой.
		//
		_eff = _effCont.AddEffect("Iskra");
		_eff->posX = mouse_pos.x + 0.f;
		_eff->posY = mouse_pos.y + 0.f;
		_eff->Reset();
	}
	else
	{
		//
		// При нажатии на левую кнопку мыши, создаём временный эффект, который завершится сам.
		//
		if (state == GameState::GAME)
			cannon->MouseDown(mouse_pos, parser.cannonballSpeed, _effCont);
		else if (state != GameState::GAME)
		{
			ParticleEffectPtr eff = _effCont.AddEffect("FindItem2");
			eff->posX = mouse_pos.x + 0.f;
			eff->posY = mouse_pos.y + 0.f;
			eff->Reset();
		}
		// Изменяем значение с 0 на 1 и наоборот.
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
		// Если эффект создан, то перемещаем его в позицию мыши.
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
		// Если эффект создан, то при отпускании мыши завершаем его.
		//
		_eff->Finish();
		_eff = NULL;
	}
}

void GameWidget::AcceptMessage(const Message& message)
{
	//
	// Виджету могут посылаться сообщения с параметрами.
	//

	const std::string& publisher = message.getPublisher();
	const std::string& data = message.getData();
}

void GameWidget::KeyPressed(int keyCode)
{
	//
	// keyCode - виртуальный код клавиши.
	// В качестве значений для проверки нужно использовать константы VK_.
	//
	if (state != GameState::GAME)
	{
		switch (keyCode)
		{
		case VK_RETURN:
			if (state != GameState::GAMEOVER)
			{
				FirstCall();

				if (state == GameState::WIN)
				{
					++level;
					targetsCount = parser.countTarget + 2 * level;
					targets->SetCount(targetsCount);
					timer.SetEndTime(parser.time - 2 * level);
				}

				Restart();
			}
			break;
		case VK_SPACE:
			if (!firstCall && state != GameState::GAMEOVER)
				Restart();
			break;
		case VK_Q:
			saver.Write(1);
			parser.Parse();

			Init();
			Restart();

			break;
		case VK_ESCAPE:
			exit(0);
			break;
		}
	}
}

void GameWidget::CharPressed(int unicodeChar)
{
	//
	// unicodeChar - Unicode код введённого символа
	//

	if (unicodeChar == L'а') {
		exit(1);
		// Реакция на ввод символа 'а'
	}
}

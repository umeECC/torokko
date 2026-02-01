#include "System/Graphics.h"
#include "SceneTitle.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "Sceneplay.h"
#include <imgui.h>

// ‰Šú‰»
void SceneTitle::Initialize()
{   //1”Ô
	// ƒXƒvƒ‰ƒCƒg‰Šú‰»
	spriteBG = new Sprite("Data/Sprite/tai.png");
	spriteStart = new Sprite("Data/Sprite/start.png");
	spriteTutorial = new Sprite("Data/Sprite/tutorial.png");
	audioManager = &AudioManager::Instance();

	audioManager->PlayBGM("title");
	choice = true;
	timer = 0;
}

// I—¹‰»
void SceneTitle::Finalize()
{
	delete spriteBG;
	delete spriteStart;
	delete spriteTutorial;

	spriteBG = nullptr;
	spriteStart = nullptr;
	spriteTutorial = nullptr;
	audioManager->StopBGM();
	
}

// XVˆ—
void SceneTitle::Update(float elapsedTime)
{
	timer++;

	static bool prevUp = false;
	static bool prevDown = false;
	static bool prevSpace = false;

	bool nowUp = (GetAsyncKeyState('W') & 0x8000) != 0;
	bool nowDown = (GetAsyncKeyState('S') & 0x8000) != 0;
	bool nowSpace = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

	if (nowUp && !prevUp)    choice = true;
	if (nowDown && !prevDown) choice = false;

	if (nowSpace && !prevSpace)
	{
		if (choice)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		}
		else
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new ScenePlay));
		}
	}

	prevUp = nowUp;
	prevDown = nowDown;
	prevSpace = nowSpace;
}

// •`‰æˆ—
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	float screenWidth = (float)graphics.GetScreenWidth();
	float screenHeight = (float)graphics.GetScreenHeight();

	// ”wŒi‘S‰æ–Ê
	spriteBG->Render(rc,
		0, 0, 0,
		screenWidth, screenHeight,
		0,
		1, 1, 1, 1);

	// ‘I‘ð’†‚Í 1.2”{
	float scaleStart = (choice ? 1.2f : 1.0f);
	float scaleTutorial = (!choice ? 1.2f : 1.0f);

	float width = 300.0f;
	float height = 100.0f;

	// Start
	spriteStart->Render(rc,
		775, 250, 0,
		width * scaleStart, height * scaleStart,
		0,
		1, 1, 1, 1);

	// Tutorial
	spriteTutorial->Render(rc,
		775, 450, 0,
		width * scaleTutorial, height * scaleTutorial,
		0,
		1, 1, 1, 1);
}

// GUI•`‰æ
void SceneTitle::DrawGUI()
{



}

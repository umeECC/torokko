#include "System/Graphics.h"
#include "SceneTitle.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include <imgui.h>

// 初期化
void SceneTitle::Initialize()
{   //1番
	// スプライト初期化
	sprite = new Sprite("Data/Sprite/chip_win.png");

	audioManager = &AudioManager::Instance();

	audioManager->PlayBGM("title");
	
}

// 終了化
void SceneTitle::Finalize()
{
	// スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
		audioManager->StopBGM();
	}
}

// 更新処理
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// なにかボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton =
		  GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y
		;
	if (gamePad.GetButtonDown() & anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	}
}

// 描画処理
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	// 描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	// 2Dスプライト描画
	{   //2番
		// タイトル描画
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		sprite->Render(rc,
			256, 128, 0, screenWidth - 256 * 2, screenHeight - 128 * 2,
			0,
			1, 1, 1, 1);
	}
}

// GUI描画
void SceneTitle::DrawGUI()
{

	ImGui::Begin("ImGUI");
	ImGui::SliderFloat2("scroll_direction", &sprite->scroll_direction.x, -10.0, 10.0);
	ImGui::End();


}

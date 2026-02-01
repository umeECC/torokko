#include "SceneOver.h"
#include "System/Input.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include <System/Graphics.h>

void SceneOver::Initialize()
{
    sprite = new Sprite("Data/Sprite/gameover.png");

    audioManager = &AudioManager::Instance();

    audioManager->PlayBGM("over");
}

void SceneOver::Finalize()
{
    delete sprite;
    sprite = nullptr;
    audioManager->StopBGM();
}

void SceneOver::Update(float)
{
    // Aボタン or Enter でリトライ
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        SceneManager::Instance().ChangeScene(new SceneTitle());
    }
}

void SceneOver::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    RenderState* renderState = graphics.GetRenderState();

    //描画準備
    RenderContext rc;
    rc.deviceContext = dc;
    rc.renderState = graphics.GetRenderState();

    //2Dスプライト描画
    {
        //タイトル描画
        float screenWidth = static_cast<float>(graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(graphics.GetScreenHeight());
        sprite->Render(rc, 0, 0, 0, screenWidth, screenHeight, 0, 1, 1, 1, 1);


    }
}

void SceneOver::DrawGUI()
{
}

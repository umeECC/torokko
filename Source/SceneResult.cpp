// SceneResult.cpp



#include "SceneResult.h"
#include "System/Graphics.h"
#include "System/Input.h"
#include "SceneTitle.h"
#include <imgui.h>
#include <SceneManager.h>
#include <SceneGame.h>

void SceneResult::Initialize() {
    //スプライト初期化
    sprite = new Sprite("Data/Sprite/gcgn.png");

    audioManager = &AudioManager::Instance();
    audioManager->PlayBGM("clear");
}
void SceneResult::Finalize() {
    //スプライト終了化
    if (sprite != nullptr)
    {
        delete sprite;
        sprite = nullptr;
        audioManager->StopBGM();
    }
}



void SceneResult::Update(float elapsedTime)
{
    // 入力でシーンを戻すなど
    //Input& input = Input::Instance();
    GamePad& gamePad = Input::Instance().GetGamePad();
    const GamePadButton anyButton = GamePad::BTN_A;
    if ((gamePad.GetButtonDown() & anyButton))
    {
        // 例：タイトルに戻る
        SceneManager::Instance().ChangeScene(new SceneTitle());
    }
}




void SceneResult::Render() {

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


void SceneResult::DrawGUI()
{
    // 今は何も描かなくてOK
}
#include "System/Graphics.h"
#include "System/Input.h"
#include "SceneTitle.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "ScenePlay.h"
#include "SceneLoading.h"
#include <algorithm>

void ScenePlay::Initialize()
{
    // ★二重初期化防止
    if (!tutorialSprites.empty()) return;

    // チュートリアル画像を読み込み
    tutorialSprites.push_back(new Sprite("Data/Sprite/tyu1.png"));
    tutorialSprites.push_back(new Sprite("Data/Sprite/tyu2.png"));
    tutorialSprites.push_back(new Sprite("Data/Sprite/tyu3.png")); 
    tutorialSprites.push_back(new Sprite("Data/Sprite/tyu4.png"));
    tutorialSprites.push_back(new Sprite("Data/Sprite/tyu5.png"));
    tutorialSprites.push_back(new Sprite("Data/Sprite/tyu6.png"));

    currentIndex = 0;      // 最初の画像
  
  
}

void ScenePlay::Finalize()
{
    for (auto sprite : tutorialSprites)
    {
        delete sprite;
    }
    tutorialSprites.clear();
}

void ScenePlay::Update(float elapsedTime)
{
    rikya += elapsedTime;

    const float INPUT_INTERVAL = 0.2f;

    int maxIndex = (int)tutorialSprites.size() - 1;

    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        if (rikya >= INPUT_INTERVAL)
        {
            rikya = 0.0f;
            currentIndex++;

            if (currentIndex > maxIndex)
            {
                // チュートリアル終了 → タイトルへ
                SceneManager::Instance().ChangeScene(new SceneTitle());
                return;
            }
        }
    }
}

void ScenePlay::Render()
{
    if (tutorialSprites.empty()) return;

    // ★範囲外アクセス防止
    if (currentIndex < 0 || currentIndex >= (int)tutorialSprites.size())
        return;

    Sprite* sprite = tutorialSprites[currentIndex];
    if (!sprite) return;

    RenderContext rc;
    rc.deviceContext = Graphics::Instance().GetDeviceContext();
    rc.renderState = Graphics::Instance().GetRenderState();

    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();

    sprite->Render(
        rc,
        0.0f, 0.0f,
        0.0f,
        screenWidth, screenHeight,
        0.0f,
        1, 1, 1, 1
    );
}

void ScenePlay::DrawGUI()
{
    // 必要なら操作説明などを表示
}

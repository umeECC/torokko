#pragma once
#include "Scene.h"
#include "System/Sprite.h"
class ScenePlay : public Scene
{
public:
    void Initialize() override;
    void Finalize() override;
    void Update(float elapsedTime) override;
    void Render() override;
    void DrawGUI() override;
    float inputDelay = 0.0f;
    int currentIndex = 0;
    std::vector<Sprite*> tutorialSprites;
private:
    Sprite* sprite = nullptr;
    float rikya;
};


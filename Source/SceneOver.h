#pragma once
#include "Scene.h"
#include "System/Sprite.h"
#include "System/AudioManager.h"
class SceneOver : public Scene
{
public:
    void Initialize() override;
    void Finalize() override;
    void Update(float elapsedTime) override;
    void Render() override;
    void DrawGUI() override;

private:
    Sprite* sprite = nullptr;
    AudioManager* audioManager = nullptr;
};


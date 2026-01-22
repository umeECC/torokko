#pragma once
#include "Scene.h"
#include "System/Sprite.h"

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
};


#pragma once
#include "Enemy.h"


class BossEnemy : public Enemy
{
public:
    BossEnemy();
    ~BossEnemy() override;
    float modelYawOffset = DirectX::XM_PI; // 180“x
    bool bossSpawned = false;     // ¶¬Ï‚İ‚©
    bool isBossBattle = false;   // ƒ{ƒXí’†‚©
    bool bossDefeated = false;   // Œ‚”jÏ‚İ‚©

    DirectX::XMFLOAT3 modelOffset = {5.5f, -70.0f, 0.0f };
    void Update(float elapsedTime) override;
    void Render(const RenderContext& rc, ModelRenderer* renderer) override;
};

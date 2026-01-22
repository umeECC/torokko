#pragma once
#include "Enemy.h"


class BossEnemy : public Enemy
{
public:
    BossEnemy();
    ~BossEnemy() override;
    float modelYawOffset = DirectX::XM_PI; // 180“x

    DirectX::XMFLOAT3 modelOffset = {5.5f, -1.0f, 0.0f };
    void Update(float elapsedTime) override;
    void Render(const RenderContext& rc, ModelRenderer* renderer) override;
};

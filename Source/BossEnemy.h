#pragma once
#include "Enemy.h"


class BossEnemy : public Enemy
{
public:
    BossEnemy();
    ~BossEnemy() override;

    float modelYawOffset = DirectX::XM_PI; // 180度

    // ★ ここ！！
    DirectX::XMFLOAT3 modelOffset = { 0.0f, 0.0f, 0.0f };

    bool bossSpawned = false;     // 生成済みか
    bool isBossBattle = false;   // ボス戦中か
    bool bossDefeated = false;   // 撃破済みか


  

    void Update(float elapsedTime) override;
    void Render(const RenderContext& rc, ModelRenderer* renderer) override;
};

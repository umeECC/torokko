#include "BossEnemy.h"
#include <d3d12.h>
#include <System/Graphics.h>

BossEnemy::BossEnemy()
{
    // ★ 必須：モデル生成
    Model* bossModel = new Model("Data/Model/Mr.Incredible/PAN.mdl");
    SetModel(bossModel);

    SetBoss(true);

    scale = { 1.0f, 1.0f, 1.0f };
}

BossEnemy::~BossEnemy()
{
    delete GetModel();
}

void BossEnemy::Update(float elapsedTime)
{
    UpdateTransform();
    GetModel()->UpdateTransform();
}
void BossEnemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    Model* model = GetModel();
    if (!model) return;

    DirectX::XMFLOAT3 originalPos = position;

    // ★ モデル専用オフセット
    position.x += modelOffset.x;
    position.y += modelOffset.y;
    position.z += modelOffset.z;

    UpdateTransform();
    model->UpdateTransform();

    renderer->Render(rc, transform, model, ShaderId::Lambert);

    position = originalPos;
}


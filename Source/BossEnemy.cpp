#include "BossEnemy.h"
#include <d3d12.h>
#include <System/Graphics.h>
BossEnemy::BossEnemy()
{
    SetModel(new Model("Data/Model/Mr.Incredible/PAN.mdl"));

    position = { 0.0f, 0.0f, 0.0f }; // ★ 必須
    scale = { 1.0f, 1.0f, 1.0f };
    angle = { 0.0f, DirectX::XM_PI, 0.0f };

    radius = 3.0f;
    height = 6.0f;
}


BossEnemy::~BossEnemy()
{

}
void BossEnemy::Update(float elapsedTime)
{
    UpdateTransform();
    GetModel()->UpdateTransform();
}


void BossEnemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    renderer->Render(rc, transform, GetModel(), ShaderId::Lambert);

    // ★ 原点確認（赤玉）
    ShapeRenderer* sr = Graphics::Instance().GetShapeRenderer();
    sr->RenderSphere(rc, position, 3.0f, {1,0,0,1});
}

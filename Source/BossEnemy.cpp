#include "BossEnemy.h"
#include <d3d12.h>
#include <System/Graphics.h>
BossEnemy::BossEnemy()
{
    SetModel(new Model("Data/Model/Mr.Incredible/PAN.mdl"));

    scale = { 3.5,3.5,3.5 };
    angle = { 0, DirectX::XM_PI, 0 };

    radius = 4.0f;
    height = 8.0f;
	SetBoss(true);
    // ★ スクショ基準の初期補正値
    modelOffset = { 5.5f, 0.0f, 0.0f };

}

BossEnemy::~BossEnemy()
{

}
void BossEnemy::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    OutputDebugStringA("Boss Hit\n");

    Enemy::AddImpulse(impulse); // ← 必須
}
void BossEnemy::Update(float elapsedTime)
{
    UpdateTransform();
    GetModel()->UpdateTransform();
}
void BossEnemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    using namespace DirectX;

    // キャラクターの transform
    XMMATRIX world = XMLoadFloat4x4(&transform);

    // モデル原点補正
    XMMATRIX offset = XMMatrixTranslation(
        modelOffset.x,
        modelOffset.y,
        modelOffset.z
    );

    XMMATRIX finalWorld = offset * world;

    XMFLOAT4X4 finalTransform;
    XMStoreFloat4x4(&finalTransform, finalWorld);

    renderer->Render(rc, finalTransform, GetModel(), ShaderId::Lambert);
}

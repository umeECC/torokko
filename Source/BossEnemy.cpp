#include "BossEnemy.h"
#include <d3d12.h>
#include <System/Graphics.h>
BossEnemy::BossEnemy()
{
    SetModel(new Model("Data/Model/Mr.Incredible/PAN.mdl"));

    radius = 3.0f;
    height = 6.0f;
   
    scale = { 2.0f, 2.0f, 2.0f };
    angle = { 0.0f, DirectX::XM_PI, 0.0f };
}

BossEnemy::~BossEnemy()
{
}

void BossEnemy::Update(float elapsedTime)
{
    UpdateTransform();
    GetModel()->UpdateTransform(); // š ‚±‚ê‚ð•K‚¸“ü‚ê‚é
}
void BossEnemy::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    //ShapeRenderer* sr = Graphics::Instance().GetShapeRenderer();

    //sr->RenderSphere(rc, position, 5.0f, { 1,0,0,1 });

    renderer->Render(rc, transform, GetModel(), ShaderId::Lambert);
}

#include "Stage.h"

// コンストラクタ
Stage::Stage()
{
	// ステージモデルを読み込み
	model = new Model("Data/Model/Stage/untitled.mdl");
	// untitled.mdl の実寸に合わせて調整
	minX = -25.0f;
	maxX = 25.0f;
	minZ = 0.0f;
	maxZ = 20.0f;
	floorY = 0.0f;

}

Stage::~Stage()
{
	// ステージモデルを破棄
	delete model;
}

// 更新処理
void Stage::Update(float elapsedTime)
{
	// 今は特にやることはない
}

// 描画処理
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());

	// レンダラにモデルを描画してもらう
	renderer->Render(rc, transform, model, ShaderId::Lambert);
}

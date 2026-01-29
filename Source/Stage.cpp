#include "Stage.h"

// コンストラクタ
Stage::Stage()
{
	// ステージモデルを読み込み
	model = new Model("Data/Model/Stage/Floormaguma/maguma2.mdl");
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

int Stage::GetCurrentAreaIndex(const DirectX::XMFLOAT3& playerPos) const
{
	// ボス部屋（Z方向の奥）
	if (playerPos.z >= 840.0f)
	{
		return 21; // ボス部屋
	}

	// 通常エリア
	return 0;
}

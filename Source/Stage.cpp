#include "Stage.h"
#include <Player.h>

// コンストラクタ
Stage::Stage()
{
	model = new Model("Data/Model/Stage/Floormaguma/maguma2.mdl");

	for (int i = 0; i < FLOOR_COUNT; ++i)
	{
		floorZ[i] = i * FLOOR_LENGTH;
	}

	floorY = -2.0f;
}

Stage::~Stage()
{
	// ステージモデルを破棄
	delete model;
}

// 更新処理
void Stage::Update(float elapsedTime)
{
	float playerZ = Player::Instance().GetPosition().z;

	for (int i = 0; i < FLOOR_COUNT; ++i)
	{
		// プレイヤーより十分後ろに行った床を前へ
		if (floorZ[i] + FLOOR_LENGTH < playerZ)
		{
			floorZ[i] += FLOOR_LENGTH * FLOOR_COUNT;
		}
	}
}


// 描画処理
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	using namespace DirectX;

	XMMATRIX S = XMMatrixScaling(0.0005f, 0.002f, 0.002f);
	XMMATRIX R = XMMatrixRotationY(XMConvertToRadians(90.0f));

	for (int i = 0; i < FLOOR_COUNT; ++i)
	{
		XMMATRIX T = XMMatrixTranslation(0.0f, floorY, floorZ[i]);

		XMFLOAT4X4 transform;
		XMStoreFloat4x4(&transform, S * R * T);

		renderer->Render(rc, transform, model, ShaderId::Lambert);
	}
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

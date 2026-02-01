#include "Stage.h"
#include <Player.h>

// コンストラクタ
Stage::Stage()
{
	floorModel = new Model("Data/Model/Stage/syokifloor.mdl");

	for (int i = 0; i < FLOOR_COUNT; ++i)
	{
		floorZ[i] = i * FLOOR_LENGTH;
	}

	floorY = -2.0f;
}

Stage::~Stage()
{
	// ステージモデルを破棄
	delete floorModel;
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

		renderer->Render(rc, transform, floorModel, ShaderId::Lambert);
	}
}

static const char* GetFloorModelPath(AreaType type)
{
	switch (type)
	{
	case AreaType::AttackGrow:      return "Data/Model/Stage/maguma2.mdl";
	case AreaType::DefenseGrow:     return "Data/Model/Stage/sabakufloor.mdl";
	case AreaType::CritRateGrow:    return "Data/Model/Stage/koorifloor.mdl";
	case AreaType::CritDamageGrow:  return "Data/Model/Stage/doukutufloor.mdl";
	case AreaType::MiniBoss:        return "Data/Model/Stage/bossfloor.mdl";
	case AreaType::Boss:            return "Data/Model/Stage/bossfloor.mdl";
	default:                        return "Data/Model/Stage/syokifloor.mdl";
	}
}


void Stage::SetAreaType(AreaType type)
{
	if (currentArea == type)
		return;

	currentArea = type;

	if (floorModel)
	{
		delete floorModel;
		floorModel = nullptr;
	}

	floorModel = new Model(GetFloorModelPath(type));
}

int Stage::GetCurrentAreaIndex(const DirectX::XMFLOAT3& playerPos) const
{
	// ボス部屋（Z方向の奥）84
	if (playerPos.z >= 840.0f)
	{
		return 21; // ボス部屋
	}

	// 通常エリア
	return 0;
}

#pragma once

#include "System/ModelRenderer.h"
#include "Player.h" // AreaType 用

class Stage
{
public:
	Stage();
	~Stage();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	// ★ エリアタイプ設定
	void SetAreaType(AreaType type);

	// ===== ステージサイズ取得 =====
	float GetMinX() const { return minX; }
	float GetMaxX() const { return maxX; }
	float GetMinZ() const { return minZ; }
	float GetMaxZ() const { return maxZ; }
	float GetFloorY() const { return floorY; }
	int GetCurrentAreaIndex(const DirectX::XMFLOAT3& playerPos) const;

private:
	// ★ 床モデル
	Model* floorModel = nullptr;
	AreaType currentArea = AreaType::None;

	// ===== ステージサイズ =====
	float minX = -20.0f;
	float maxX = 20.0f;
	float minZ = 0.0f;
	float maxZ = 40.0f;
	float floorY = -1.5f;

	static constexpr int FLOOR_COUNT = 3;
	static constexpr float FLOOR_LENGTH = 50.0f;

	float floorZ[FLOOR_COUNT];
};

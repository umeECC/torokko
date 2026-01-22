#pragma once

#include "System/ModelRenderer.h"

// ステージ
class Stage
{
public:
	Stage();
	~Stage();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	// ===== ステージサイズ取得 =====
	float GetMinX() const { return minX; }
	float GetMaxX() const { return maxX; }
	float GetMinZ() const { return minZ; }
	float GetMaxZ() const { return maxZ; }
	float GetFloorY() const { return floorY; }
	int GetCurrentAreaIndex(const DirectX::XMFLOAT3& playerPos) const;

private:
	Model* model = nullptr;

	// ===== ステージサイズ =====
	float minX = -20.0f;   // 左端
	float maxX = 20.0f;   // 右端
	float minZ = 0.0f;   // 開始位置
	float maxZ = 40.0f;   // 奥行き（←ここを変える）
	float floorY = 0.0f;   // 床の高さ
};

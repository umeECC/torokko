#pragma once

#include <DirectXMath.h>

// カメラコントローラー
class CameraController
{
public:
	// 更新処理
	void Update(float elapsedTime);

	// ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

private:
	DirectX::XMFLOAT3		target = { 0, 0, 0 };	// 注視点
	DirectX::XMFLOAT3		angle = { 45, 0, 0 };	// 回転角度
	float					rollSpeed = DirectX::XMConvertToRadians(0); // 回転速度
	float					range = 15.0f; 	// 距離
	float					maxAngleX = DirectX::XMConvertToRadians(45);
	float					minAngleX = DirectX::XMConvertToRadians(45);
};

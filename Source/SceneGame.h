#pragma once
#include "Stage.h"
#include "CameraController.h"
#include "Scene.h"

// ゲームシーン
class SceneGame : public Scene 
{
public:
	SceneGame() {};
	~SceneGame() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

	// GUI描画
	void DrawGUI() override;
	void SpawnBoss();

private:
	Stage* stage = nullptr;
	CameraController* cameraController = nullptr;
private:
	bool bossSpawned = false;    // 生成済みか
	bool bossDefeated = false;   // 撃破済みか
};

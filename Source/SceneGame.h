#pragma once
#include "Stage.h"
#include "CameraController.h"
#include "Scene.h"
#include "System/AudioManager.h"
// ƒQ[ƒ€ƒV[ƒ“
class SceneGame : public Scene 
{
public:
	SceneGame() {};
	~SceneGame() override {}

	// ‰Šú‰»
	void Initialize() override;

	// I—¹‰»
	void Finalize() override;

	// XVˆ—
	void Update(float elapsedTime) override;

	// •`‰æˆ—
	void Render() override;

	// GUI•`‰æ
	void DrawGUI() override;
	void SpawnBoss();

private:
	Stage* stage = nullptr;
	CameraController* cameraController = nullptr;
	AudioManager* audioManager = nullptr;
private:
	bool bossSpawned = false;    // ¶¬Ï‚İ‚©
	bool bossDefeated = false;   // Œ‚”jÏ‚İ‚©

	AreaType currentAreaType;
};

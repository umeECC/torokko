#pragma once

#include "System/Sprite.h"
#include "Scene.h"
#include "System/AudioManager.h"

// タイトルシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}

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

private:
	Sprite* spriteBG = nullptr;
	Sprite* spriteStart = nullptr;
	Sprite* spriteTutorial = nullptr;
	bool choice = true; // true = Start, false = Tutorial
	int timer = 0;
	AudioManager* audioManager = nullptr;
};

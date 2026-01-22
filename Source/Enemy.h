#pragma once

#include "System/ModelRenderer.h"
#include "Character.h"

// エネミー
class Enemy : public Character
{
public:
	Enemy() {}
	~Enemy() override {}

	// 破棄
	void Destroy();

	// 更新処理
	virtual void Update(float elapsedTime) = 0;
	Model* GetModel() const { return model; }
	void SetModel(Model* model);
	void SetBoss(bool boss);
	bool IsBoss() const { return isBoss; }
	// 描画処理
	virtual void Render(const RenderContext& rc, ModelRenderer* renderer) = 0;
private:
	Model* model = nullptr;
	bool isBoss = false;
};

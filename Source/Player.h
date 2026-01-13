#pragma once

#include "System/ModelRenderer.h"
#include "System/AudioSource.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"


enum class AreaType
{
	None,
	AttackGrow,
	DefenseGrow,
	CritRateGrow,
	CritDamageGrow,
	BalancedGrow,
	Jackpot
};





struct PlayerStatus
{
	float hp = 100.0f;           //HP
	float attack = 10.0f;        // 攻撃力
	float defense = 5.0f;        // 防御力
	float critDamage = 1.5f;     // クリティカル倍率
	float critRate = 0.05f;      // クリティカル率（0.0 ～ 1.0）
};



// プレイヤー
class Player : public Character
{
private:
	Player() {}
	~Player() override {};

public:
	// インスタンス取得
	static Player& Instance()
	{
		static Player instance;
		return instance;
	}

	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	// デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;

	// デバッグ用GUI描画
	void DrawDebugGUI();

	const PlayerStatus& GetStatus() const { return status; }


	void BeginAreaChoice();


	//ステータス設定
	void ApplyAreaGrowth(AreaType area);
private:
	// スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	// 移動入力処理
	void InputMove(float elapsedTime);

	// プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

	// 弾丸と敵の衝突処理
	void CollisionProjectilesVsEnemies();

	// ジャンプ入力処理
	void InputJump();

	// 弾丸入力処理
	void InputProjectile();



protected:
	// 着地した時に呼ばれる
	void OnLanding() override;




private:
	Model*	model = nullptr;
	float	moveSpeed = 5.0f;
	float	turnSpeed = DirectX::XMConvertToRadians(720);
	float				jumpSpeed = 12.0f;
	int					jumpCount = 0;
	int					jumpLimit = 2;
	ProjectileManager	projectileManager;
	Effect*				hitEffect = nullptr;
	AudioSource*		hitSE = nullptr;

	PlayerStatus status;

	int currentAreaIndex = -1;



	// ===== エリア成長関連 =====
	bool isChoosingAreaBonus = false;   // 今、成長選択中か
	AreaType choiceA = AreaType::None;  // 左候補
	AreaType choiceB = AreaType::None;  // 右候補
	AreaType selectedArea = AreaType::None; // 現在選ばれている方
	AreaType lastSelectedArea = AreaType::None; // 前回確定したエリア


	float areaDecisionZ = 0.0f;         // このZを超えたら確定

};

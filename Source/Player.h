#pragma once

#include "System/ModelRenderer.h"
#include "System/AudioSource.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"
#include <Stage.h>
#include <System/Sprite.h>
enum class TrolleyChoice
{
	None,
	Left,
	Right
};

enum class AreaType
{
	None,
	AttackGrow,
	DefenseGrow,
	CritRateGrow,
	CritDamageGrow,
	BalancedGrow,

	MiniBoss,   // ★中ボスエリア
	Boss        // ★ボスエリア
};

struct TrolleyOption
{
	Sprite* sprite;
	AreaType areaType;

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

	void SelectRandomTrolleyImages();

	AreaType GetRandomGrowArea();

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

	void SetStage(Stage* stage) { this->stage = stage; }

	void BeginAreaChoice();
	float selectedScale = 1.1f;
	float unselectedScale = 0.95f;

	float selectedColor = 1.3f;  // 明るく
	float unselectedColor = 0.6f;  // 暗く

	void StartMiniBossBattle();

	void StartBossBattle();

	void UpdateAutoBattle(float elapsedTime);

	void OnEnemyDefeated();



	//ステータス設定
	void ApplyAreaGrowth(AreaType area);
private:
	// スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;
	//bool isAtStaheEnd = false;
	// トロッコ問題用フラグ
	std::vector<Sprite*> trolleySprites;   // 候補5枚
	std::vector<TrolleyOption> trolleyOptions;

	TrolleyOption* leftOption = nullptr;
	TrolleyOption* rightOption = nullptr;

	bool showTrolleyUI = false;
	bool trolleyChosen = false;
	Sprite* trolleySprite = nullptr;

	//Sprite* trolleySprite = nullptr;

	TrolleyChoice trolleyChoice = TrolleyChoice::None;
	//ID3D11ShaderResourceView* endImageSRV = nullptr;
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
	Stage* stage = nullptr;
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



	// ===== バトル管理 =====
	bool isInBattle = false;     // 今戦闘中？
	bool isBossBattle = false;  // ボス戦か？

	float battleTimer = 0.0f;   // 自動戦闘用タイマー

	// 敵ステータス
	float enemyHP = 0.0f;
	float enemyAttack = 0.0f;
	float enemyDefense = 0.0f;

};

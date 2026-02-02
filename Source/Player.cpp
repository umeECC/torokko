#include <imgui.h>
#include "Player.h"
#include "System/Input.h"
#include "System/Audio.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "Stage.h"
#include <algorithm>
#include <d3d11.h>
#include <System/Graphics.h>
#include <random>
#include <SceneManager.h>
#include "SceneResult.h"
#include "SceneOver.h"
#include <BossEnemy.h>
#include <EnemySlime.h>


void Player::SelectRandomTrolleyImages()
{
	if (trolleyOptions.size() < areaChoiceCount) return;

	static std::random_device rd;
	static std::mt19937 gen(rd());

	TrolleyOption* miniBossOption = nullptr;
	std::vector<TrolleyOption*> normalOptions;

	for (auto& opt : trolleyOptions)
	{
		if (opt.areaType == AreaType::MiniBoss)
			miniBossOption = &opt;
		else if (opt.areaType != AreaType::Boss)
			normalOptions.push_back(&opt);
	}

	std::shuffle(normalOptions.begin(), normalOptions.end(), gen);

	// ===== 2択 =====
	if (areaChoiceCount == 2)
	{
		optionA = normalOptions[0];
		optionB = normalOptions[1];
		optionC = nullptr;
		return;
	}

	// ===== 3択 =====
	optionA = normalOptions[0];
	optionB = normalOptions[1];

	// ★ 7・14だけ中ボス確定
	if (isMiniBossChoiceArea && miniBossOption)
	{
		optionC = miniBossOption;
	}
	else
	{
		optionC = normalOptions[2];
	}
}


static const float AREA_LENGTH = 40.0f;
static const int AREA_COUNT = 0;



AreaType Player::GetRandomGrowArea()
{
	int r = rand() % 4; 
	switch (r)
	{
	case 0: return AreaType::AttackGrow;//火山
	case 1: return AreaType::DefenseGrow;//砂漠
	case 2: return AreaType::CritRateGrow;//氷山
	case 3: return AreaType::CritDamageGrow;//洞窟
	case 4: return AreaType::MiniBoss;
	case 5: return AreaType::Boss;
	default: return AreaType::AttackGrow;
	}
}




// 初期化
void Player::Initialize() 
{
	model = new Model("Data/Model/Mr.Incredible/MineCart.mdl");

	hito = new Model("Data/Model/hito/fbx/fbx file.mdl");
	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.5f;
	

	// ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");

	// ヒットSE読み込み
	hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");

	status.hp = 100.0f;
	status.attack = 10.0f;
	status.defense = 5.0f;
	status.critRate = 0.05f;
	status.critDamage = 1.5f;
	
	hpFrameSprite = new Sprite("Data/Sprite/体力ゲージ.png");
	hpBarSprite = new Sprite("Data/Sprite/体力.png");
	attackIconSprite = new Sprite("Data/Sprite/拳.png");
	defenseIconSprite = new Sprite("Data/Sprite/盾.png");
	critIconSprite = new Sprite("Data/Sprite/会心.png");
	critDamageIconSprite = new Sprite("Data/Sprite/会心ダメ.png");

	trolleyOptions.push_back({ new Sprite("Data/Sprite/火山.png"), AreaType::AttackGrow });
	trolleyOptions.push_back({ new Sprite("Data/Sprite/砂漠.png"), AreaType::DefenseGrow });
	trolleyOptions.push_back({ new Sprite("Data/Sprite/氷山.png"), AreaType::CritRateGrow });
	trolleyOptions.push_back({ new Sprite("Data/Sprite/洞窟.png"), AreaType::CritDamageGrow });
	trolleyOptions.push_back({ new Sprite("Data/Sprite/中ボス部屋.png"), AreaType::MiniBoss });
	trolleyOptions.push_back({ new Sprite("Data/Sprite/ボス部屋.png"), AreaType::Boss });
	//trolleyOptions.push_back({ new Sprite("Data/Sprite/Jackpot.png"), AreaType::Jackpot });

	bossHpFrameSprite = new Sprite("Data/Sprite/体力ゲージ.png");
	bossHpBarSprite = new Sprite("Data/Sprite/体力.png");

	for (int i = 0; i < 10; ++i)
	{
		numberSprites[i] = new Sprite(
			("Data/Sprite/" + std::to_string(i) + ".png").c_str()
		);
	}

	percentSprite = new Sprite("Data/Sprite/%.png");
	xSprite = new Sprite("Data/Sprite/×.png");
	dotSprite = new Sprite("Data/Sprite/ドット.png");
	upArrowSprite = new Sprite("Data/Sprite/上矢印.png");
	
	prevStatus = status;
}

// 終了化
void Player::Finalize()
{
	// オプションスプライト解放
	for (auto& opt : trolleyOptions)
	{
		delete opt.sprite;
	}
	trolleyOptions.clear();

	delete hitSE;
	hitSE = nullptr;

	delete hitEffect;
	hitEffect = nullptr;

	delete model;
	model = nullptr;

	// ★ 状態を全部リセット
	status = {};
	position = { 0,0,0 };
	velocity = { 0,0,0 };
	

	currentAreaIndex = 20;
	lastSelectedArea = AreaType::None;
	selectedArea = AreaType::None;

	isInBattle = false;
	isBossBattle = false;
	showStageImage = false;
	isChoosingAreaBonus = false;

	jumpCount = 0;
	battleTimer = 0.0f;


	delete hpFrameSprite;
	delete hpBarSprite;
	delete attackIconSprite;
	delete defenseIconSprite;
	delete critIconSprite;
	delete critDamageIconSprite;
	for (int i = 0; i < 10; ++i)
	{
		delete numberSprites[i];
		numberSprites[i] = nullptr;
	}

	delete percentSprite;
	delete xSprite;
	delete dotSprite;
	delete upArrowSprite;

	delete bossHpFrameSprite;
	delete bossHpBarSprite;
	bossHpFrameSprite = nullptr;
	bossHpBarSprite = nullptr;

}

bool Player::IsFinaBossBattle() const
{
	return isBossBattle;
}

// 更新処理
void Player::Update(float elapsedTime)
{
	prevStatus = status;

	// ===== ノックバック更新（最優先）=====
	UpdateKnockback(elapsedTime);

	// ===== バトル中 =====
	if (isInBattle)
	{
		// ★ 追加
		if (!isKnockback)
		{
			InputMove(elapsedTime);   // ← これが無かった
		}

		UpdateVelocity(elapsedTime);

		if (!isKnockback)
		{
			UpdateAutoBattle(elapsedTime);
		}

		UpdateTransform();
		model->UpdateTransform();
		return;
	}




	
	if (isKnockback)
	{
		knockbackTimer -= elapsedTime;
		if (knockbackTimer <= 0.0f)
		{
			isKnockback = false;
		}
	}

	if (stage)
	{
		if (position.z >= stage->GetMaxZ() && !showTrolleyUI && !trolleyChosen)
		{
			showTrolleyUI = true;
			SelectRandomTrolleyImages();
		}
	}

	static bool bossSpawned = false;

	if (!bossSpawned && position.z > 845.0f)
	{
		EnemyManager::Instance().SpawnBossVisual();
		bossSpawned = true;
	}
	char buf[64];
	sprintf_s(buf, "Player Z = %.1f\n", position.z);
	OutputDebugStringA(buf);



	// ★ ステージ画像の表示制御
	if (isChoosingAreaBonus)
	{
		if (position.z >= stageImageStartZ && position.z <= stageImageEndZ)
		{
			showStageImage = true;
		}
		else
		{
			showStageImage = false;
		}
	}

	// オブジェクト行列を更新

	UpdateTransform();

	// モデル行列更新
	model->UpdateTransform();




	// ===== ① エリア選択中の入力処理 =====
	if (isChoosingAreaBonus && showStageImage)
	{
		GamePad& gamePad = Input::Instance().GetGamePad();

		// A（左）
		if ((GetAsyncKeyState('A') & 0x8000) || (gamePad.GetButtonDown() & GamePad::BTN_A))
		{
			if (optionA)
				selectedArea = optionA->areaType;
		}
		// D（右）
		if ((GetAsyncKeyState('D') & 0x8000) || (gamePad.GetButtonDown() & GamePad::BTN_D))
		{
			if (optionB)
				selectedArea = optionB->areaType;
		}
		// C（中央 / 3択目）
		if (areaChoiceCount == 3)
		{
			if ((GetAsyncKeyState('W') & 0x8000) || (gamePad.GetButtonDown() & GamePad::BTN_Y))
			{
				if (optionC)
					selectedArea = optionC->areaType;
			}
		}
	}



	// ===== ② 通常移動・行動（止めない）=====
	InputMove(elapsedTime);
	InputJump();
	InputProjectile();

	UpdateVelocity(elapsedTime);

	projectileManager.Update(elapsedTime);
	
	CollisionPlayerVsEnemies();
	CollisionProjectilesVsEnemies();

	UpdateTransform();
	model->UpdateTransform();

	// ===== ③ エリア侵入判定 =====

	int areaIndex = static_cast<int>(position.z / AREA_LENGTH);

	if (areaIndex != currentAreaIndex)
	{
		currentAreaIndex = areaIndex;

		stageImageShown = false;
		


		
		if (areaIndex == 21)
		{
			StartBossBattle();
		}
		else
		{
			BeginAreaChoice();
		}


	}



	// ===== ④ 確定判定 =====
	if (isChoosingAreaBonus && position.z >= areaDecisionZ)
	{
		ApplyAreaGrowth(selectedArea);
		isChoosingAreaBonus = false;

	}
	if (showStageImage)
	{
		GamePad& gamePad = Input::Instance().GetGamePad();

		if (gamePad.GetButtonDown() & GamePad::BTN_A && optionA)
		{
			ApplyAreaGrowth(optionA->areaType);
		}
		else if (gamePad.GetButtonDown() & GamePad::BTN_D && optionB)
		{
			ApplyAreaGrowth(optionB->areaType);
		}
		else if (areaChoiceCount == 3 && (gamePad.GetButtonDown() & GamePad::BTN_Y) && optionC)
		{
			ApplyAreaGrowth(optionC->areaType);
		}
	}

	
}


// 描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	using namespace DirectX;

	// ===== トロッコ =====
	XMMATRIX cartM =
		XMMatrixScaling(scale.x, scale.y, scale.z) *
		XMMatrixRotationY(angle.y + XMConvertToRadians(90.0f)) *
		XMMatrixTranslation(position.x, position.y, position.z);

	XMFLOAT4X4 cartWorld;
	XMStoreFloat4x4(&cartWorld, cartM);

	renderer->Render(rc, cartWorld, model, ShaderId::Lambert);

	// ===== hito =====
	float hitoScale = 0.12f;

	XMMATRIX hitoM =
		XMMatrixScaling(hitoScale, hitoScale, hitoScale) *
		XMMatrixRotationY(angle.y) *   // ← 90度足さない
		XMMatrixTranslation(position.x, position.y + 0.2f, position.z);

	XMFLOAT4X4 hitoWorld;
	XMStoreFloat4x4(&hitoWorld, hitoM);

	renderer->Render(rc, hitoWorld, hito, ShaderId::Lambert);
	// ===== UI描画 =====
	DrawHPGauge(rc);          // プレイヤー左上

	DrawEnemyStatus(rc);      // MiniBoss / Boss 共通（右上）

	// エリア選択UI
	if (showStageImage && optionA && optionB)
	{
		float screenW = 1280.0f;
		float screenH = 720.0f;
		float baseW = 400.0f;
		float baseH = 300.0f;

		// A（左）
		{
			bool selected = (selectedArea == optionA->areaType);
			float scale = selected ? 0.9f : 0.6f;
			float color = selected ? 0.9f : 0.6f;

			optionA->sprite->Render(
				rc,
				screenW * 0.25f - baseW * 0.5f,
				screenH * 0.65f - baseH * 0.5f,
				0,
				baseW * scale,
				baseH * scale,
				0,
				color, color, color, 1.0f
			);
		}

		// B（右）
		{
			bool selected = (selectedArea == optionB->areaType);
			float scale = selected ? 0.9f : 0.6f;
			float color = selected ? 0.9f : 0.6f;

			optionB->sprite->Render(
				rc,
				screenW * 0.85f - baseW * 0.5f,
				screenH * 0.65f - baseH * 0.5f,
				0,
				baseW * scale,
				baseH * scale,
				0,
				color, color, color, 1.0f
			);
		}

		// C（3択）
		if (areaChoiceCount == 3 && optionC)
		{
			bool selected = (selectedArea == optionC->areaType);
			float scale = selected ? 0.9f : 0.6f;
			float color = selected ? 0.9f : 0.6f;

			optionC->sprite->Render(
				rc,
				screenW * 0.575f - baseW * 0.6f,
				screenH * 0.3f - baseH * 0.6f,
				0,
				baseW * scale,
				baseH * scale,
				0,
				color, color, color, 1.0f
			);
		}
	}
}



bool Player::IsInBossRoom() const
{
	if (!stage) return false;

	int areaIndex = stage->GetCurrentAreaIndex(position);

	// ボスエリア候補を可視化
	//("[BossCheck] Z=%.1f Area=%d\n", position.z, areaIndex);

	return areaIndex >= 20; // ← 仮
}
void Player::DrawEnemyStatus(const RenderContext& rc)
{
	if (!isInBattle) return;

	const float baseX = 20.0f;
	const float baseY = 20.0f;
	const float screenW = 1280.0f;

	// プレイヤーUIと同じサイズ
	const float gaugeW = 300.0f;
	const float gaugeH = 40.0f;
	const float iconSize = 50.0f;
	const float iconSpace = 10.0f;
	const float numberSize = 24.0f;

	float x = screenW - baseX - gaugeW;
	float y = baseY;

	// ===== ラベル =====
	DrawNumber(
		rc,
		x,
		y - 28.0f,
		isBossBattle ? "BOSS" : "MINI",
		24.0f
	);

	// ===== HP =====
	float hpRate = enemyHP / maxEnemyHP;
	hpRate = std::clamp(hpRate, 0.0f, 1.0f);

	hpFrameSprite->Render(
		rc,
		x,
		y,
		0,
		gaugeW,
		gaugeH,
		0,
		1, 1, 1, 1
	);

	hpBarSprite->Render(
		rc,
		x,
		y,
		0,
		gaugeW * hpRate,
		gaugeH,
		0,
		1, 0.3f, 0.3f, 1
	);

	// ===== ステータス =====
	float iconX = x;
	float iconY = y + gaugeH + 16.0f;

	// 攻撃
	attackIconSprite->Render(
		rc,
		iconX,
		iconY,
		0,
		iconSize,
		iconSize,
		0,
		1, 1, 1, 1
	);
	DrawNumber(
		rc,
		iconX + iconSize + 8.0f,
		iconY,
		std::to_string((int)enemyAttack),
		numberSize
	);

	// 防御
	iconY += iconSize + iconSpace;
	defenseIconSprite->Render(
		rc,
		iconX,
		iconY,
		0,
		iconSize,
		iconSize,
		0,
		1, 1, 1, 1
	);
	DrawNumber(
		rc,
		iconX + iconSize + 8.0f,
		iconY,
		std::to_string((int)enemyDefense),
		numberSize
	);
}

	
void Player::DrawHPGauge(const RenderContext& rc)
{
	const float baseX = 20.0f;
	const float baseY = 20.0f;

	const float gaugeW = 300.0f;
	const float gaugeH = 40.0f;

	// ===== HPゲージ =====
	float hpRate = std::clamp(status.hp / 100.0f, 0.0f, 1.0f);



	hpFrameSprite->Render(
		rc,
		baseX,
		baseY,
		0,
		gaugeW,
		gaugeH,
		0,
		1, 1, 1, 1
	);

	hpBarSprite->Render(
		rc,
		baseX,
		baseY,
		0,
		gaugeW * hpRate,
		gaugeH,
		0,
		1, 1, 1, 1
	);

	// ===== ステータス縦並び =====
	const float iconSize = 50.0f;
	const float iconSpace = 10.0f;

	float iconX = baseX;
	float iconY = baseY + gaugeH + 16.0f;

	// 攻撃
	attackIconSprite->Render(
		rc,
		iconX,
		iconY,
		0,
		iconSize,
		iconSize,
		0,
		1, 1, 1, 1
	);
	DrawNumber(
		rc,
		iconX + iconSize + 8.0f,
		iconY,
		std::to_string((int)status.attack),
		24.0f
	);

	if (IsIncreased(status.attack, prevStatus.attack))
	{
		upArrowSprite->Render(
			rc,
			iconX + iconSize + 80.0f,
			iconY,
			0,
			24, 24,
			0,
			1, 1, 1, 1
		);
	}

	// 防御
	iconY += iconSize + iconSpace;
	defenseIconSprite->Render(
		rc,
		iconX,
		iconY,
		0,
		iconSize,
		iconSize,
		0,
		1, 1, 1, 1
	);
	DrawNumber(
		rc,
		iconX + iconSize + 8.0f,
		iconY,
		std::to_string((int)status.defense),
		24.0f
	);

	// 会心
	iconY += iconSize + iconSpace;
	critIconSprite->Render(
		rc,
		iconX,
		iconY,
		0,
		iconSize,
		iconSize,
		0,
		1, 1, 1, 1
	);
	std::string critRateText =
		std::to_string((int)(status.critRate * 100)) + "%";

	DrawNumber(
		rc,
		iconX + iconSize + 8.0f,
		iconY,
		critRateText,
		24.0f
	);

	// 会心ダメージ
	iconY += iconSize + iconSpace;
	critDamageIconSprite->Render(
		rc,
		iconX,
		iconY,
		0,
		iconSize,
		iconSize,
		0,
		1, 1, 1, 1
	);

	int critDmgInt = (int)(status.critDamage * 10);
	std::string critDamageText =
		std::to_string(critDmgInt / 10) + "." +
		std::to_string(critDmgInt % 10) + "x";

	DrawNumber(
		rc,
		iconX + iconSize + 8.0f,
		iconY,
		critDamageText,
		24.0f
	);


	// ↑
	if (IsIncreased(status.attack, prevStatus.attack))
	{
		upArrowSprite->Render(
			rc,
			iconX + iconSize + 80.0f,
			iconY,
			0,
			24.0f,
			24.0f,
			0,
			1, 1, 1, 1
		);
	}
}
	
void Player::DrawNumber(
	const RenderContext& rc,
	float x,
	float y,
	const std::string& text,
	float size
)
{
	float drawX = x;

	for (char c : text)
	{
		Sprite* sprite = nullptr;
		float advance = size * 0.8f;   // ← 基本間隔（全体を少し広め）

		if (c >= '0' && c <= '9')
		{
			sprite = numberSprites[c - '0'];
			advance = size * 0.8f;
		}
		else if (c == '%')
		{
			sprite = percentSprite;
			advance = size * 0.9f;     // ← % は少し広め
		}
		else if (c == 'x')
		{
			sprite = xSprite;
			advance = size * 0.8f;
		}
		else if (c == '.')
		{
			sprite = dotSprite;
			advance = size * 0.7f;    // ← ドットは狭く
		}

		if (!sprite) continue;

		sprite->Render(
			rc,
			drawX,
			y,
			0,
			size,
			size,
			0,
			1, 1, 1, 1
		);

		drawX += advance;
	}
}



bool Player::IsIncreased(float current, float prev) const
{
	return current > prev;
}


// デバッグプリミティブ描画

void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	// 基底クラスの関数呼び出し
	Character::RenderDebugPrimitive(rc, renderer);

	// 弾丸デバッグプリミティブ描画
	projectileManager.RenderDebugPrimitive(rc, renderer);
}

void Player::DrawDebugGUI()
{
	
}

void Player::StartMiniBossBattle()
{
	isInBattle = true;
	isBossBattle = false;
	isMiniBossBattle = true;

	maxEnemyHP = 50.0f + currentAreaIndex * 15.0f;
	enemyHP = maxEnemyHP;

	enemyAttack = 5.0f + currentAreaIndex * 1.0f;
	enemyDefense = 3.0f + currentAreaIndex * 1.0f;
}

void Player::StartBossBattle()
{
	if (isBossBattle) return; // ★ 追加

	isInBattle = true;
	isBossBattle = true;
	isMiniBossBattle = false;
	isChoosingAreaBonus = false;
	showStageImage = false;
	stageImageShown = true;

	optionA = nullptr;
	optionB = nullptr;
	optionC = nullptr;
	maxEnemyHP = BOSS_HP;
	enemyHP = maxEnemyHP;

	enemyAttack = 50.0f;
	enemyDefense = 60.0f;
}


// スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	// 入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	// カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// 移動ベクトルはXZ平面に水平なベクトルになるようにする

	// カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		// 単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		// 単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// スティックの水平入力値をカメラ右方向に反映し、
	// スティックの垂直入力値をカメラ前方向に反映し、
	// 進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	// Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

// 移動入力処理
void Player::InputMove(float elapsedTime)
{
	if (isKnockback) return; // ★ これ最重要

	Move(elapsedTime, 0.0f, 1.0f, moveSpeed);

}
void Player::UpdateKnockback(float elapsedTime)
{
	if (!isKnockback) return;

	knockbackTimer -= elapsedTime;

	if (knockbackTimer <= 0.0f)
	{
		isKnockback = false;  // ← ★これが実行されてる？
		OutputDebugStringA("Knockback End\n");
	}
}


// プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	OutputDebugStringA("CollisionPlayerVsEnemies called\n");

	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition))
		{
			// 敵の真上付近に当たったかを判定
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);
			// 上から踏んづけた場合は小ジャンプする
			if (normal.y > 0.8f)
			{
				// 小ジャンプする
				Jump(jumpSpeed * 0.5f);
			}
			else
			{
				if (isKnockback) return;

				DirectX::XMFLOAT3 dir =
				{
					position.x - enemy->GetPosition().x,
					0.0f,
					position.z - enemy->GetPosition().z
				};

				float len = sqrtf(dir.x * dir.x + dir.z * dir.z);
				if (len > 0.0f)
				{
					dir.x /= len;
					dir.z /= len;
				}
				
				AddImpulse({ dir.x * 15.0f, 8.0f, dir.z * 15.0f });
				StartKnockback(0.25f);
			}

		}

	}
}

// 弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				// ダメージを与える
				if (enemy->ApplyDamage(1, 0.5f))
				{
					// 吹き飛ばす
					{
						DirectX::XMFLOAT3 impulse;
						const float power = 10.0f;
						const DirectX::XMFLOAT3& e = enemy->GetPosition();
						const DirectX::XMFLOAT3& p = projectile->GetPosition();
						float vx = e.x - p.x;
						float vz = e.z - p.z;
						float lengthXZ = sqrtf(vx * vx + vz * vz);
						vx /= lengthXZ;
						vz /= lengthXZ;

						impulse.x = vx * power;
						impulse.y = power * 0.5f;
						impulse.z = vz * power;

						enemy->AddImpulse(impulse);
					}


					




					// 弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}
}

// ジャンプ入力処理
void Player::InputJump()
{
	//// ボタン入力でジャンプ（ジャンプ回数制限つき）
	//GamePad& gamePad = Input::Instance().GetGamePad();
	//if (gamePad.GetButtonDown() & GamePad::BTN_A)
	//{
	//	// ジャンプ回数制限
	//	if (jumpCount < jumpLimit)
	//	{
	//		// ジャンプ
	//		jumpCount++;
	//		Jump(jumpSpeed);
	//	}
	//}
}

// 弾丸入力処理
void Player::InputProjectile()
{
	
}

void Player::StartKnockback(float time)
{
	isKnockback = true;
	knockbackTimer = time;
}


// 着地した時に呼ばれる
void Player::OnLanding()
{
	jumpCount = 0;
}



void Player::ApplyAreaGrowth(AreaType area)
{
	float growthMultiplier = 1.0f;
	showStageImage = false;     // ★これ必須
	isChoosingAreaBonus = false;
	stageImageChosen = true;
	// ===== 同じエリア連続チェック =====
	if (area == lastSelectedArea && area != AreaType::None)
	{
		// HP減少
		status.hp -= 10.0f;
		if (status.hp < 0.0f)
		{
			SceneManager::Instance().ChangeScene(new SceneOver());
		}

		// 成長2倍
		growthMultiplier = 2.5f;
	}

	// ===== 成長処理 =====
	switch (area)
	{
	case AreaType::AttackGrow:
		status.attack += 6.0f * growthMultiplier;
		break;

	case AreaType::DefenseGrow:
		status.defense += 3.0f * growthMultiplier;
		break;

	case AreaType::CritRateGrow:
		status.critRate += 0.02f * growthMultiplier;
		if (status.critRate > 0.9f)
			status.critRate = 0.9f;
		break;

	case AreaType::CritDamageGrow:
		status.critDamage += 0.2f * growthMultiplier;
		break;

	case AreaType::MiniBoss:
	{
		StartMiniBossBattle();

		if (bosskau == 0)
		{
			EnemySlime* slime = new EnemySlime();
			slime->SetPosition(DirectX::XMFLOAT3(0, 0, 325));
			slime->SetTerritory(slime->GetPosition(), 10.0f);
			EnemyManager& enemyManager = EnemyManager::Instance();
			enemyManager.Register(slime);
		}
	
		if (bosskau == 1)
		{
			EnemySlime* slime = new EnemySlime();
			slime->SetPosition(DirectX::XMFLOAT3(0, 0, 605));
			slime->SetTerritory(slime->GetPosition(), 10.0f);
			EnemyManager& enemyManager = EnemyManager::Instance();
			enemyManager.Register(slime);
		}

	
		
		break;
	}

	default:
		break;
	}

	// ★ 最後に必ず更新
	lastSelectedArea = area;
	if (stage)
	{
		stage->SetAreaType(area);
	}
}




void Player::BeginAreaChoice()
{
	isChoosingAreaBonus = true;

	// 7以降は3択、それまでは2択
	if (currentAreaIndex >= 7)
		areaChoiceCount = 3;
	else
		areaChoiceCount = 2;

	// ★ 中ボス確定フラグ
	isMiniBossChoiceArea =
		(currentAreaIndex == 7 || currentAreaIndex == 14);

	if (currentAreaIndex == 8)
	{
		bosskau += 1;
	}

	SelectRandomTrolleyImages();
	selectedArea = optionA->areaType;

	// ===== 表示区間 =====
	stageImageStartZ = position.z + AREA_LENGTH * 0.5f;
	stageImageEndZ = position.z + AREA_LENGTH * 1.0f;

	// ===== 確定位置（★ここ）=====
	areaDecisionZ = position.z + AREA_LENGTH * 0.9f;

	showStageImage = false; // 最初は非表示
}


//void Player::StartBossBattle()
//{
//	isInBattle = true;
//	isBossBattle = true;
//	isMiniBossBattle = false;
//
//
//
//	maxEnemyHP = BOSS_HP;
//	enemyHP = maxEnemyHP;
//
//	enemyAttack = 40.0f;
//	enemyDefense = 25.0f;
//}
void Player::UpdateAutoBattle(float elapsedTime)
{
	battleTimer += elapsedTime;
	if (battleTimer < 0.5f)
		return;

	battleTimer = 0.0f;

	// ===== プレイヤー攻撃 =====
	float damage;
	// クリティカル
	if ((rand() / (float)RAND_MAX) < status.critRate)
	{
		damage = status.attack * status.critDamage;
	}
	else
	{
		damage = status.attack - enemyDefense;
	}
    
	if (damage < 0.0f) damage = 1.0f;

	// ヒットSE再生
	hitSE->Play(false);
	// ヒットエフェクト再生
	{
		DirectX::XMFLOAT3 e = Player::position;
		e.y += 0.5;
		hitEffect->Play(e);
	}


	
	enemyHP -= damage;

	if (enemyHP <= 0.0f)
	{
		OnEnemyDefeated();
		return;
	}

	// ===== 敵の攻撃 =====
	float enemyDamage = enemyAttack - status.defense;
	if (enemyDamage < 0.0f) enemyDamage = 1.0f;

	status.hp -= enemyDamage;
	if (status.hp < 0.0f)
	{
		SceneManager::Instance().ChangeScene(new SceneOver());
	}
		
}


void Player::OnEnemyDefeated()
{
	isInBattle = false;
	// ★ 全敵を消す（中ボス想定）
	EnemyManager& mgr = EnemyManager::Instance();


	for (int i = 0; i < mgr.GetEnemyCount(); ++i)
	{
		Enemy* e = mgr.GetEnemy(i);
		e->Destroy();
	}
	if (!isBossBattle)
	{
		// 中ボス報酬（全部上がる）
		status.attack += 5.0f;
		status.defense += 5.0f;
		status.critRate += 0.05f;
		status.critDamage += 0.5f;
		status.hp += 20.0f;
		if (status.hp > 100.0f)
		{
			status.hp == 100.0f;
		}

	
	
	}
	else
	{
		// ボス撃破（クリア）
		// リザルト画面など
		SceneManager::Instance().ChangeScene(new SceneResult());
	}
}


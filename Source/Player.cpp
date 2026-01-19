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

void Player::SelectRandomTrolleyImages()
{
	if (trolleyOptions.size() < 2) return;

	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, trolleyOptions.size() - 1);

	int a = dist(gen);
	int b;
	do
	{
		b = dist(gen);
	} while (b == a);

	leftOption = &trolleyOptions[a];
	rightOption = &trolleyOptions[b];
}



static const float AREA_LENGTH = 50.0f;
static const int AREA_COUNT = 7;



static AreaType GetRandomGrowArea()
{
	int r = rand() % 5; // Jackpot除外
	switch (r)
	{
	case 0: return AreaType::AttackGrow;
	case 1: return AreaType::DefenseGrow;
	case 2: return AreaType::CritRateGrow;
	case 3: return AreaType::CritDamageGrow;
	case 4: return AreaType::BalancedGrow;
	default: return AreaType::AttackGrow;
	}
}




// 初期化
void Player::Initialize() 
{
	model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	// ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");

	// ヒットSE読み込み
	hitSE = Audio::Instance().LoadAudioSource("Data/Sound/Hit.wav");

	status.hp = 100.0f;
	status.attack = 10.0f;
	status.defense = 5.0f;
	status.critRate = 0.05f;
	status.critDamage = 1.5f;
	

	trolleyOptions.push_back({ new Sprite("Data/Sprite/火山.png"), AreaType::AttackGrow });
	trolleyOptions.push_back({ new Sprite("Data/Sprite/砂漠.png"), AreaType::DefenseGrow });
	trolleyOptions.push_back({ new Sprite("Data/Sprite/氷山.png"), AreaType::CritRateGrow });
	trolleyOptions.push_back({ new Sprite("Data/Sprite/洞窟.png"), AreaType::CritDamageGrow });
	//trolleyOptions.push_back({ new Sprite("Data/Sprite/Jackpot.png"), AreaType::Jackpot });

}

// 終了化
void Player::Finalize()
{
	for (auto& opt : trolleyOptions)
	{
		delete opt.sprite;
	}
	trolleyOptions.clear();

	delete hitSE;
	delete hitEffect;
	delete model;
}

// 更新処理
void Player::Update(float elapsedTime)
{

	
	
	if (stage)
	{
		if (position.z >= stage->GetMaxZ() && !showTrolleyUI && !trolleyChosen)
		{
			showTrolleyUI = true;
			SelectRandomTrolleyImages();
		}
	}


	

	// オブジェクト行列を更新

	UpdateTransform();

	// モデル行列更新
	model->UpdateTransform();


	// ===== �G���A�N������ =====

	// ===== ① エリア選択中の入力処理 =====
	if (isChoosingAreaBonus)
	{
		

		if (GetAsyncKeyState('A') & 0x8000)
		{
			selectedArea = choiceA;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			selectedArea = choiceB;
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

		ApplyAreaGrowth(GetRandomGrowArea());
		BeginAreaChoice();
	}

	// ===== ④ 確定判定 =====
	if (isChoosingAreaBonus && position.z >= areaDecisionZ)
	{
		ApplyAreaGrowth(selectedArea);
		isChoosingAreaBonus = false;

	}
	if (showTrolleyUI)
	{
		GamePad& gamePad = Input::Instance().GetGamePad();

		if (gamePad.GetButtonDown() & GamePad::BTN_A)
		{
			ApplyAreaGrowth(leftOption->areaType);
			trolleyChosen = true;
			showTrolleyUI = false;
			position.x -= 10.0f;
		}
		else if (gamePad.GetButtonDown() & GamePad::BTN_B)
		{
			ApplyAreaGrowth(rightOption->areaType);
			trolleyChosen = true;
			showTrolleyUI = false;
			position.x += 10.0f;
		}
	}


}

// 描画処理
void Player::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	renderer->Render(rc, transform, model, ShaderId::Lambert);

	// 弾丸描画処理

	projectileManager.Render(rc, renderer);

	if (showTrolleyUI)
	{
		float screenW = 1280.0f;
		float screenH = 720.0f;

		float baseW = 400.0f;
		float baseH = 300.0f;

		// 左
		{
			bool selected = (selectedArea == leftOption->areaType);
			float scale = selected ? 1.1f : 0.95f;
			float color = selected ? 1.3f : 0.6f;

			float w = baseW * scale;
			float h = baseH * scale;

			leftOption->sprite->Render(
				rc,
				screenW * 0.25f - w * 0.5f,
				screenH * 0.5f - h * 0.5f,
				0,
				w, h,
				0,
				color, color, color, 1.0f
			);
		}

		// 右
		{
			bool selected = (selectedArea == rightOption->areaType);
			float scale = selected ? 1.1f : 0.95f;
			float color = selected ? 1.3f : 0.6f;

			float w = baseW * scale;
			float h = baseH * scale;

			rightOption->sprite->Render(
				rc,
				screenW * 0.75f - w * 0.5f,
				screenH * 0.5f - h * 0.5f,
				0,
				w, h,
				0,
				color, color, color, 1.0f
			);
		}

		}
}




// デバッグプリミティブ描画

void Player::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	// 基底クラスの関数呼び出し
	Character::RenderDebugPrimitive(rc, renderer);

	// 弾丸デバッグプリミティブ描画
	projectileManager.RenderDebugPrimitive(rc, renderer);
}

// デバッグ用GUI描画
void Player::DrawDebugGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->GetWorkPos();
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);



	auto DrawAreaText = [](AreaType type)
		{
			switch (type)
			{
			case AreaType::AttackGrow: ImGui::Text("Attack Up"); break;
			case AreaType::DefenseGrow: ImGui::Text("Defense Up"); break;
			case AreaType::CritRateGrow: ImGui::Text("Crit Rate Up"); break;
			case AreaType::CritDamageGrow: ImGui::Text("Crit Damage Up"); break;
			case AreaType::BalancedGrow: ImGui::Text("Balanced Up"); break;
			case AreaType::Jackpot: ImGui::Text("JACKPOT!!"); break;
			default: break;
			}
		};









	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		// トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 位置
			ImGui::InputFloat3("Position", &position.x);
			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			// スケール
			ImGui::InputFloat3("Scale", &scale.x);
		}
	}
		if (ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("HP        : %.1f", status.hp);
			ImGui::Text("Attack        : %.1f", status.attack);
			ImGui::Text("Defense       : %.1f", status.defense);
			ImGui::Text("Crit Rate     : %.1f%%", status.critRate * 100.0f);
			ImGui::Text("Crit Damage   : x%.2f", status.critDamage);
		}


		ImGui::Text("Area Index : %d", currentAreaIndex);


		if (isChoosingAreaBonus)
		{
			ImGui::Separator();
			ImGui::Text("Choose Area Growth");
			ImGui::Text("LEFT / RIGHT to switch");
			ImGui::Text("Auto confirm soon");

			ImGui::Separator();

			// === Choice A ===
			if (selectedArea == choiceA)
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "▶ A");
			else
				ImGui::Text("  A");

			ImGui::SameLine();
			DrawAreaText(choiceA);

			// === Choice B ===
			if (selectedArea == choiceB)
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "▶ B");
			else
				ImGui::Text("  B");

			ImGui::SameLine();
			DrawAreaText(choiceB);

			ImGui::Separator();
			ImGui::Text("Current Selected :");

			DrawAreaText(selectedArea);


			ImGui::Text("LastArea: ");
			DrawAreaText(lastSelectedArea);
			
		}


	ImGui::End();
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
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	Player::position.z += 0.05f;

	//if (Player::position.z >= 50.0f)
	//{
	//	Player::position.z = 0.0f;
	//}
}

// プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
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
				// 押し出し後の位置設定
				enemy->SetPosition(outPosition);
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
					// ヒットエフェクト再生
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}
					// ヒットSE再生
					hitSE->Play(false);

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
	// ボタン入力でジャンプ（ジャンプ回数制限つき）
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// ジャンプ回数制限
		if (jumpCount < jumpLimit)
		{
			// ジャンプ
			jumpCount++;
			Jump(jumpSpeed);
		}
	}
}

// 弾丸入力処理
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// 直進弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);
		// 発射位置（プレイヤーの腰あたり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
		// 発射
		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
	}
	// 追尾弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		// 発射位置（プレイヤーの腰あたり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		// ターゲット（デフォルトではプレイヤーの前方）
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		// 一番近くの敵をターゲットにする
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			// 敵との距離判定
			Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
			float d;
			DirectX::XMStoreFloat(&d, D);
			if (d < dist)
			{
				dist = d;
				target = enemy->GetPosition();
				target.y += enemy->GetHeight() * 0.5f;
			}
		}

		// 発射
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);
	}
}

// 着地した時に呼ばれる
void Player::OnLanding()
{
	jumpCount = 0;
}



void Player::ApplyAreaGrowth(AreaType area)
{
	float growthMultiplier = 1.0f;

	// ===== 同じエリア連続チェック =====
	if (area == lastSelectedArea && area != AreaType::None)
	{
		// HP減少
		status.hp -= 10.0f;
		if (status.hp < 0.0f)
			status.hp = 0.0f;

		// 成長1.5倍
		growthMultiplier = 1.5f;
	}

	// ===== 成長処理 =====
	switch (area)
	{
	case AreaType::AttackGrow:
		status.attack += 3.0f * growthMultiplier;
		break;

	case AreaType::DefenseGrow:
		status.defense += 2.0f * growthMultiplier;
		break;

	case AreaType::CritRateGrow:
		status.critRate += 0.02f * growthMultiplier;
		if (status.critRate > 0.8f)
			status.critRate = 0.8f;
		break;

	case AreaType::CritDamageGrow:
		status.critDamage += 0.25f * growthMultiplier;
		break;

	case AreaType::BalancedGrow:
		status.attack += 1.5f * growthMultiplier;
		status.defense += 1.5f * growthMultiplier;
		status.critRate += 0.01f * growthMultiplier;
		status.critDamage += 0.15f * growthMultiplier;
		break;

	case AreaType::Jackpot:
		status.attack += 8.0f * growthMultiplier;
		status.defense += 6.0f * growthMultiplier;
		status.critRate += 0.08f * growthMultiplier;
		status.critDamage += 0.5f * growthMultiplier;
		break;

	default:
		break;
	}

	// ★ 最後に必ず更新
	lastSelectedArea = area;
}




void Player::BeginAreaChoice()
{
	isChoosingAreaBonus = true;

	// 成長候補2つ生成
	choiceA = GetRandomGrowArea();
	do
	{
		choiceB = GetRandomGrowArea();
	} while (choiceB == choiceA);

	// デフォルトはA
	selectedArea = choiceA;

	// 確定Z（少し先）
	areaDecisionZ = position.z + AREA_LENGTH * 0.9f;
}

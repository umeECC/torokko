#include "System/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "Player.h"
#include "EffectManager.h"
#include <BossEnemy.h>

// 初期化
void SceneGame::Initialize()
{
	// ステージ初期化
	stage = new Stage();

	// プレイヤー初期化
	Player::Instance().Initialize();


	audioManager = &AudioManager::Instance();

	//audioManager->PlayBGM("stage", true);


	// ★ ステージをプレイヤーにセット
	Player::Instance().SetStage(stage);

	// カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),	// 視点
		DirectX::XMFLOAT3(0, 0, 0),	// 注視点
		DirectX::XMFLOAT3(0, 1, 0)		// 上方向
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 視野角
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),	// 画面アスペクト比
		0.1f,	// クリップ距離(近)
		1000.0f	// クリップ距離(遠)
	);

	// カメラコントローラー初期化
	cameraController = new CameraController();

	// エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();

	
	

		

}

// 終了化
void SceneGame::Finalize()
{
	// エネミー終了化
	EnemyManager::Instance().Clear();

	// カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	// ステージ終了化
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}

	// プレイヤー終了化
	Player::Instance().Finalize();

	audioManager->StopBGM();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// カメラ更新
	DirectX::XMFLOAT3 target = Player::Instance().GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	stage->Update(elapsedTime);

	Player& player = Player::Instance();

	// ★ 強制Z判定（これが基準）
	if (!bossSpawned && player.GetPosition().z > 840.0f)
	{
		SpawnBoss();
	}

	Player::Instance().Update(elapsedTime);
	EnemyManager::Instance().Update(elapsedTime);
	EffectManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ModelRenderer* modelRenderer = graphics.GetModelRenderer();

	// 描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f };	// ライト方向（下方向）
	rc.renderState = graphics.GetRenderState();

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3Dモデル描画
	{
		// ステージ描画
		stage->Render(rc, modelRenderer);

		// プレイヤー描画
		Player::Instance().Render(rc, modelRenderer);

		// エネミー描画
		EnemyManager::Instance().Render(rc, modelRenderer);

		// エフェクト描画
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		// プレイヤーデバッグプリミティブ描画
		Player::Instance().RenderDebugPrimitive(rc, shapeRenderer);

		// エネミーデバッグプリミティブ描画
		EnemyManager::Instance().RenderDebugPrimitive(rc, shapeRenderer);
	}

	// 2Dスプライト描画
	{

	}
}

// GUI描画
void SceneGame::DrawGUI()
{
	// プレイヤーデバッグ描画
	Player::Instance().DrawDebugGUI();
}

void SceneGame::SpawnBoss()
{
	if (bossSpawned) return;

	BossEnemy* boss = new BossEnemy();

	boss->SetPosition({ 0.0f, 0.0f, 845.0f }); // ★ 固定

	EnemyManager::Instance().Register(boss);

	bossSpawned = true;
}

#include "EnemyManager.h"
#include "Collision.h"
#include "BossEnemy.h"
#include <Windows.h>
#include "Player.h"   // ★ 必須

// 更新処理
void EnemyManager::Update(float elapsedTime)
{
	// EnemyManager::Update(int count = (int)enemies.size(); // ← ★この行
	int count = (int)enemies.size();
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	// 破棄処理
	// ※enemiesの範囲for文中でerase()すると不具合が発生してしまうため、
	// 　更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Enemy* enemy : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
		if (it != enemies.end())
		{
			enemies.erase(it);
		}

		// 削除
		delete enemy;
	}
	// 破棄リストをクリア
	removes.clear();

	// 敵同士の衝突処理
	CollisionEnemyVsEnemies();
}
void EnemyManager::SpawnBossVisual()
{

	BossEnemy* boss = new BossEnemy();
	boss->SetPosition({
		0.0f,
		boss->GetHeight() * 0.5f,
		900.0f
		});


	enemies.push_back(boss);
}
void EnemyManager::ClearEnemies()
{
	for (Enemy* e : enemies)
	{
		delete e; // ★ Model は触らない
	}
	enemies.clear();
}


// 描画処理
void EnemyManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{

	char buf[64];
	sprintf_s(buf, "Enemy count = %d\n", (int)enemies.size());
	OutputDebugStringA(buf);
	OutputDebugStringA("\n");


	for (Enemy* enemy : enemies)
	{
		enemy->Render(rc, renderer);
	}
}


// エネミー登録
void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

// エネミー削除
void EnemyManager::Remove(Enemy* enemy)
{
	// 破棄リストに追加
	removes.insert(enemy);
}

// エネミー全削除
void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}

// デバッグプリミティブ描画
void EnemyManager::RenderDebugPrimitive(
	const RenderContext& rc,
	ShapeRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
		// ★ 赤い球（原点確認用）
		renderer->RenderSphere(
			rc,
			enemy->GetPosition(),
			3.0f,
			{ 1, 0, 0, 1 }
		);

		// 既存のデバッグ描画（あれば）
		enemy->RenderDebugPrimitive(rc, renderer);
	}
}

// エネミー同士の衝突処理
void EnemyManager::CollisionEnemyVsEnemies()
{
	size_t enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemyA = enemies.at(i);
		for (int j = i + 1; j < enemyCount; ++j)
		{
			Enemy* enemyB = enemies.at(j);

			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectCylinderVsCylinder(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				enemyA->GetHeight(),
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				enemyB->GetHeight(),
				outPosition))
			{
				enemyB->SetPosition(outPosition);
			}
		}
	}
}

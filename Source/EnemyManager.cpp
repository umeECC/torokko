#include "EnemyManager.h"
#include "Collision.h"
#include "BossEnemy.h"

#include "Player.h"   // ★ 必須
void EnemyManager::PlayerVsEnemies()
{
	Player& player = Player::Instance();

	// ★ ボス戦中は押し返さない
	if (player.IsBossBattle())
		return;

	if (player.IsKnockback())
		return;

	DirectX::XMFLOAT3 playerPos = player.GetPosition();

	for (Enemy* enemy : enemies)
	{
		DirectX::XMFLOAT3 outPos;

		if (Collision::IntersectCylinderVsCylinder(
			playerPos,
			player.GetRadius(),
			player.GetHeight(),
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPos))
		{
			// ノックバック処理（通常敵のみ）
			DirectX::XMFLOAT3 dir = {
				playerPos.x - enemy->GetPosition().x,
				0.0f,
				playerPos.z - enemy->GetPosition().z
			};

			float len = sqrtf(dir.x * dir.x + dir.z * dir.z);
			if (len > 0.0f)
			{
				dir.x /= len;
				dir.z /= len;
			}

			player.AddImpulse({ dir.x * 15.0f, 8.0f, dir.z * 15.0f });
			player.StartKnockback(0.25f);
			return;
		}
	}
}



// 更新処理
void EnemyManager::Update(float elapsedTime)
{
	// EnemyManager::Update(int count = (int)enemies.size(); // ← ★この行


	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}
	PlayerVsEnemies();

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


	DirectX::XMFLOAT3 p = Player::Instance().GetPosition();
	boss->SetPosition({
		p.x,
		p.y,
		p.z + 10.0f   // ← 円の中心付近
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
	printf("Enemy count = %d\n", (int)enemies.size());

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
void EnemyManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (Enemy* enemy : enemies)
	{
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

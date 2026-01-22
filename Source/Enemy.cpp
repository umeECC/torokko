#include "Enemy.h"
#include "EnemyManager.h"

// ”jŠü
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
void Enemy::SetModel(Model* m)
{
	model = m;
}

void Enemy::SetBoss(bool boss)
{
	isBoss = boss;
}

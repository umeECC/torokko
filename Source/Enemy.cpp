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
void Enemy::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

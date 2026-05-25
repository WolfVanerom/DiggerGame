#include "EnemySpawnManager.h"
#include "EnemySpawnUpdateComponent.h"

namespace dae
{
	EnemySpawnUpdateComponent::EnemySpawnUpdateComponent(GameObject* pOwner)
		: Component(pOwner)
	{
	}

	void EnemySpawnUpdateComponent::Update(float deltaTime)
	{
		m_enemySpawnManager.Update(deltaTime);
	}
}
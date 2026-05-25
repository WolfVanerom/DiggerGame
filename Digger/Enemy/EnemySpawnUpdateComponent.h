#pragma once
#include "Component.h"

namespace dae
{
	class EnemySpawnManager;
	class EnemySpawnUpdateComponent final : public Component
	{
	public:
		EnemySpawnUpdateComponent(GameObject* pOwner);
		virtual ~EnemySpawnUpdateComponent() = default;
		EnemySpawnUpdateComponent(const EnemySpawnUpdateComponent& other) = delete;
		EnemySpawnUpdateComponent(EnemySpawnUpdateComponent&& other) = delete;
		EnemySpawnUpdateComponent& operator=(const EnemySpawnUpdateComponent& other) = delete;
		EnemySpawnUpdateComponent& operator=(EnemySpawnUpdateComponent&& other) = delete;
		void Update(float deltaTime) override;
	protected:
		EnemySpawnManager& m_enemySpawnManager{ EnemySpawnManager::GetInstance() };
	private:
	};
}
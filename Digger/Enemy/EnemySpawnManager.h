#pragma once
#include <vector>
#include <Singleton.h>
#include <HitBoxComponent.h>

namespace dae
{
	class EnemyComponent;
	class EnemyState;
	class Scene;
	class PlayerComponent;

	class EnemySpawnManager final
	{
	public:
		void Init(Scene* scene, PlayerComponent* playerComponent);

		void Update(float deltaTime);

		void SpawnEnemy(Scene* scene);
		void SwitchPauseSpawning();
		void PauseEnemies();
		void RemoveEnemy(EnemyComponent* enemy);
		void ClearEnemies();

		void SetMaxEnemyCountForLevel(int level);

		const std::vector<EnemyComponent*>& GetEnemies() const;

	private:
		int m_enemyCount{ 0 };
		int m_currentMaxEnemyCount{ 4 };
		std::vector<EnemyComponent*> m_pEnemies{};
		std::vector<HitBoxComponent*> m_pEnemyHitboxes{};
		std::vector<int> m_MaxEnemyCountPerLevel{ 4, 5, 6, 7, 8 };
		float m_SpawnTimer{ 0.0f };
		bool m_SpawningPaused{ true };
		Scene* m_pScene{ nullptr };
		PlayerComponent* m_pPlayerComponent{ nullptr };
	};
}
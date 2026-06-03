#pragma once
#include <vector>
#include <Singleton.h>

namespace dae
{
	class EnemyComponent;
	class EnemyState;
	class Scene;
	class PlayerComponent;

	class EnemySpawnManager final : public Singleton<EnemySpawnManager>
	{
	public:
		void Init(Scene* scene, PlayerComponent* playerComponent);

		void Update(float deltaTime);

		void SpawnEnemy(Scene* scene);
		void RemoveEnemy(EnemyComponent* enemy);
		void ClearEnemies();
		void SpawnBonusCherry();

		void SetMaxEnemyCountForLevel(int level);

		// Added accessor for enemy list
		const std::vector<EnemyComponent*>& GetEnemies() const { return m_pEnemies; }

	private:
		int enemyCount{ 0 };
		int currentMaxEnemyCount{ 4 };
		std::vector<EnemyComponent*> m_pEnemies{};
		std::vector<int> m_MaxEnemyCountPerLevel{ 4, 5, 6, 7, 8 };
		float m_SpawnTimer{ 0.0f };
		Scene* m_pScene{ nullptr };
		PlayerComponent* m_pPlayerComponent{ nullptr };
	};
}
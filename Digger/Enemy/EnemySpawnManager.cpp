#include "EnemySpawnManager.h"
#include <Scene.h>
#include <GameObject.h>
#include "EnemyComponent.h"


namespace dae
{
	void EnemySpawnManager::Init(Scene* scene, PlayerComponent* playerComponent)
	{
		m_pScene = scene;
		m_pPlayerComponent = playerComponent;
	}

	void EnemySpawnManager::Update(float deltaTime)
	{
		m_SpawnTimer += deltaTime;
		if (m_SpawnTimer >= 5.0f)
		{
			m_SpawnTimer = 0.0f;
			EnemyState* newState = nullptr;
			SpawnEnemy(newState, m_pScene);
		}
	}

	void EnemySpawnManager::SpawnEnemy(EnemyState* state, Scene* scene)
	{
		if (enemyCount < currentMaxEnemyCount)
		{
			auto go = std::make_unique<GameObject>();
			auto textureComponent = std::make_unique<TextureComponent>(go.get());
			textureComponent->SetTexture("media/cnob1.png");
			auto enemyComponent = std::make_unique<EnemyComponent>(go.get(), m_pPlayerComponent, textureComponent.get());
			scene->Add(std::move(go));
			m_pEnemies.push_back(enemyComponent.get());
			enemyCount++;
		}
	}
	void EnemySpawnManager::RemoveEnemy(EnemyComponent* enemy)
	{
		enemy->GetParent()->MarkForDeletion();
		auto it = std::find(m_pEnemies.begin(), m_pEnemies.end(), enemy);
		if (it != m_pEnemies.end())
		{
			m_pEnemies.erase(it);
			enemyCount--;
		}
	}
	void EnemySpawnManager::ClearEnemies()
	{
		for (auto* enemy : m_pEnemies)
		{
			enemy->GetParent()->MarkForDeletion();
		}
		m_pEnemies.clear();
		enemyCount = 0;
	}
	void EnemySpawnManager::SpawnBonusCherry()
	{
		if (enemyCount == currentMaxEnemyCount)
		{
			// Spawn bonus cherry logic here
		}
	}
}
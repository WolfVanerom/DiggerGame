#include "EnemySpawnManager.h"
#include <Scene.h>
#include <GameObject.h>
#include "EnemyComponent.h"
#include "LevelManager.h"
#include <HitBoxComponent.h>

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
			SpawnEnemy(m_pScene);
		}
	}

	void EnemySpawnManager::SpawnEnemy(Scene* scene)
	{
		if (scene == nullptr or enemyCount >= currentMaxEnemyCount or m_SpawningPaused)
		{
			return;
		}

		auto go = std::make_unique<GameObject>();

		auto textureComponent = std::make_unique<TextureComponent>(go.get());
		textureComponent->SetTexture("media/cnob1.png");
		textureComponent->SetDrawSize(LevelManager::m_tileWidth, LevelManager::m_tileHeight);

		auto enemyComponent = std::make_unique<EnemyComponent>(go.get(), m_pPlayerComponent, textureComponent.get());
		auto* enemyPtr = enemyComponent.get();

		auto hitboxComponent = std::make_unique<HitBoxComponent>(go.get(), dae::HitboxLayer::Enemy, glm::vec2(16, 16), glm::vec2(0, 0));

		go->addComponent(std::move(textureComponent));
		go->addComponent(std::move(hitboxComponent));
		go->addComponent(std::move(enemyComponent));

		scene->Add(std::move(go));
		m_pEnemies.push_back(enemyPtr);
		enemyCount++;
	}

	void EnemySpawnManager::SwitchPauseSpawning()
	{
		m_SpawningPaused = !m_SpawningPaused;
	}

	void EnemySpawnManager::PauseEnemies()
	{
		for (auto* enemy : m_pEnemies)
		{
			enemy->SetStateFromType(dae::EnemyStateType::Pause);
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

	void EnemySpawnManager::SetMaxEnemyCountForLevel(int level)
	{
		currentMaxEnemyCount = m_MaxEnemyCountPerLevel[level-1];
	}
}

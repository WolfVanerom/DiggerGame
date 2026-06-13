#include "EnemySpawnManager.h"
#include <Scene.h>
#include <GameObject.h>
#include "EnemyComponent.h"
#include "LevelManager.h"
#include <HitBoxComponent.h>
#include "PlayerComponent.h"

namespace dae
{
	void EnemySpawnManager::Init(Scene* scene, PlayerComponent* playerComponent)
	{
		m_pScene = scene;
		m_pPlayerComponent = playerComponent;
	}

	void EnemySpawnManager::Update(float deltaTime)
	{
		if (serviceLocator::GetGameDataManager().GetGameMode() == GameMode::Versus)
		{
			return;
		}

		m_SpawnTimer += deltaTime;
		if (m_SpawnTimer >= 5.0f)
		{
			m_SpawnTimer = 0.0f;
			SpawnEnemy(m_pScene);
		}

		for (auto* m_pEnemyHitbox : m_pEnemyHitboxes)
		{
			if (m_pEnemyHitbox == nullptr)
			{
				continue;
			}
			for (auto* other : HitBoxComponent::CheckAll())
			{
				if (other == nullptr || other == m_pEnemyHitbox) {
					continue;
				}
				if (!m_pEnemyHitbox->Overlaps(*other)) {
					continue;
				}
				if (other->GetLayer() == HitboxLayer::Item)
				{
					auto enemy = m_pEnemyHitbox->GetParent();
					enemy->MarkForDeletion();
					m_pEnemies.erase(std::remove(m_pEnemies.begin(), m_pEnemies.end(), static_cast<EnemyComponent*>(enemy->getComponent(typeid(EnemyComponent)))), m_pEnemies.end());
					m_pEnemyHitboxes.erase(std::remove(m_pEnemyHitboxes.begin(), m_pEnemyHitboxes.end(), m_pEnemyHitbox), m_pEnemyHitboxes.end());
					m_enemyCount--;
					return;
				}
			}
		}
	}

	void EnemySpawnManager::SpawnEnemy(Scene* scene)
	{
		if (scene == nullptr or m_enemyCount >= m_currentMaxEnemyCount or m_SpawningPaused)
		{
			return;
		}

		auto go = std::make_unique<GameObject>();

		auto textureComponent = std::make_unique<TextureComponent>(go.get());
		textureComponent->SetTexture("media/cnob1.png");
		textureComponent->SetDrawSize(LevelManager::m_tileWidth, LevelManager::m_tileHeight);

		auto enemyComponent = std::make_unique<EnemyComponent>(go.get(), m_pPlayerComponent, textureComponent.get());
		auto* enemyPtr = enemyComponent.get();

		auto hitboxComponent = std::make_unique<HitBoxComponent>(go.get(), dae::HitboxLayer::Enemy, glm::vec2(LevelManager::m_tileWidth, LevelManager::m_tileHeight), glm::vec2(0, 0));
		auto hitboxComponentPtr = hitboxComponent.get();

		go->addComponent(std::move(textureComponent));
		go->addComponent(std::move(hitboxComponent));
		go->addComponent(std::move(enemyComponent));

		scene->Add(std::move(go));
		m_pEnemies.push_back(enemyPtr);
		m_pEnemyHitboxes.push_back(hitboxComponentPtr);
		m_enemyCount++;
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
			m_enemyCount--;
			m_pPlayerComponent->AddScore(250);
		}
	}

	void EnemySpawnManager::ClearEnemies()
	{
		for (auto* enemy : m_pEnemies)
		{
			enemy->GetParent()->MarkForDeletion();
		}
		m_pEnemies.clear();
		m_pEnemyHitboxes.clear();
		m_enemyCount = 0;
	}

	void EnemySpawnManager::SetMaxEnemyCountForLevel(int level)
	{
		m_currentMaxEnemyCount = m_MaxEnemyCountPerLevel[level-1];
	}

	const std::vector<EnemyComponent*>& EnemySpawnManager::GetEnemies() const
	{
		return m_pEnemies;
	}
}

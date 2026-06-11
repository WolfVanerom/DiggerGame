#pragma once
#include "Component.h"
#include "Observer.h"
#include <Enemy/EnemySpawnManager.h>
#include <Player/PlayerAccessor.h>
#include "ServiceLocator.h"

namespace dae
{
	class PlayerComponent;
	class TextComponent;
	class LifeComponent final : public Component, public Observer
	{
		PlayerComponent* m_playerComponent{};
		TextComponent* m_textComponent{};
		EnemySpawnManager& m_enemySpawnManager{ dae::serviceLocator::GetEnemySpawnManager() };
		PlayerAccessor& m_playerAccessor{ dae::serviceLocator::GetPlayerAccessor() };
		soundSystem& m_soundSystem{ dae::serviceLocator::GetSoundSystem() };
	public:
		LifeComponent(GameObject* pOwner, PlayerComponent* playerComponent, TextComponent* textComponent);
		void Update(float deltaTime) override;
		void OnNotify(Event event, GameObject* gameObject) override;
	};
}
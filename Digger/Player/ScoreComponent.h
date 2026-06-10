#pragma once
#include "Component.h"
#include "Observer.h"
#include <ServiceLocator.h>
#include "GameDataManager.h"

namespace dae
{
	class PlayerComponent;
	class TextComponent;
	class ScoreComponent final : public Component, public Observer
	{
	private:
		PlayerComponent* m_playerComponent{};
		TextComponent* m_textComponent{};
		GameDataManager& m_gameDataManager{ serviceLocator::GetGameDataManager() };
	public:
        ScoreComponent(GameObject* pOwner, PlayerComponent* playerComponent, TextComponent* textComponent);
		void Update(float deltaTime) override;
		void OnNotify(Event event, GameObject* gameObject) override;
	};
}
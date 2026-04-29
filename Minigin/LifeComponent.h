#pragma once
#include "Component.h"
#include "Observer.h"

namespace dae
{
	class PlayerComponent;
	class TextComponent;
	class LifeComponent final : public Component, public Observer
	{
		PlayerComponent* m_playerComponent{};
		TextComponent* m_textComponent{};
	public:
		LifeComponent(GameObject* pOwner, PlayerComponent* playerComponent, TextComponent* textComponent);
		void Update(float deltaTime) override;
		void OnNotify(Event event, GameObject* gameObject) override;
	};
}
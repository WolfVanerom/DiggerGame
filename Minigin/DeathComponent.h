#pragma once
#include "Component.h"
#include "Observer.h"

namespace dae
{
	class PlayerComponent;
	class TextComponent;
	class DeathComponent final : public Component, public Observer
	{
		PlayerComponent* m_playerComponent{};

	public:
		DeathComponent(GameObject* pOwner, PlayerComponent* playerComponent);
		void Update(float deltaTime) override;
		void OnNotify(Event event, GameObject* gameObject) override;
	};
}

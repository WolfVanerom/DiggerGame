#include "DeathComponent.h"
#include <iostream>

namespace dae {
	DeathComponent::DeathComponent(GameObject* pOwner, PlayerComponent* playerComponent)
		: Component{ pOwner }
		, m_playerComponent{ playerComponent }
	{
	}
	void DeathComponent::Update(float) {
	}
	void DeathComponent::OnNotify(Event event, GameObject*)
	{
		if (event != Event::PlayerDied)
			return;
		std::cout << "Player has died!" << std::endl;
	}
}
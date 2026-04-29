#include "DeathComponent.h"
#include <iostream>

namespace dae {
	DeathComponent::DeathComponent(GameObject* pOwner)
		: Component{ pOwner }
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
#pragma once
#include "Component.h"
#include "Observer.h"

namespace dae
{
	class TextComponent;
	class DeathComponent final : public Component, public Observer
	{
	public:
		DeathComponent(GameObject* pOwner);
		void Update(float deltaTime) override;
		void OnNotify(Event event, GameObject* gameObject) override;
	};
}

#pragma once
#include "Component.h"
#include <GameDataManager.h>

namespace dae
{
	class PlayerComponent;
	class HitBoxComponent;
	class PlayerDamageComponent final : public Component
	{
	public:
		PlayerDamageComponent(GameObject* owner, PlayerComponent* player, HitBoxComponent* hitbox);
		void Update(float dt) override;
	private:
		PlayerComponent* m_player;
		HitBoxComponent* m_hitbox;
		float m_invulnrebilityTimer{};
		GameMode m_currentGameMode{};
		int m_playerNumber{};
	};
}
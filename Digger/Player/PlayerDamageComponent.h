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
		void ReleaseInvulnerability();
	private:
		PlayerComponent* m_player;
		HitBoxComponent* m_hitbox;
		bool m_isInvulnerable{ false };
		GameMode m_currentGameMode{};
		int m_playerNumber{};
	};
}
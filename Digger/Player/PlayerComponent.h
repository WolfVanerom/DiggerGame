#pragma once
#include "Component.h"
#include "Observer.h"
#include <memory>
#include "LevelManager.h"

namespace dae
{

	class PlayerComponent final : public Component, public subject
	{
		int m_health{ 5 };
		int m_score{ 0 };
		int m_previousCellX{ -1 };
		int m_previousCellY{ -1 };
		float m_previousWorldX{ 0.f };
		float m_previousWorldY{ 0.f };
		float m_tunnelProgress{ 0.f };
		bool m_isInTunnel{ false };
		bool m_hasPreviousCell{ false };
		TunnelDirection m_previousMovementDirection{ TunnelDirection::none };
		TunnelDirection m_lockedMovementDirection{ TunnelDirection::none };

		LevelManager& m_levelManager{ LevelManager::GetInstance() };
	public:
		PlayerComponent(GameObject* pOwner);
		~PlayerComponent() override;
		PlayerComponent(const PlayerComponent& other) = delete;
		PlayerComponent(PlayerComponent&& other) = delete;
		PlayerComponent& operator=(const PlayerComponent& other) = delete;
		PlayerComponent& operator=(PlayerComponent&& other) = delete;
		void Update(float deltaTime) override;

		void SubtractHealth(int amount);
		int GetHealth() const { return m_health; }
		int GetScore() const { return m_score; }
		TunnelDirection GetLockedMovementDirection() const { return m_lockedMovementDirection; }
		bool CanMoveInDirection(TunnelDirection direction) const
		{
			return m_lockedMovementDirection == TunnelDirection::none || m_lockedMovementDirection == direction;
		}
		void SetLockedMovementDirection(TunnelDirection direction) {
			if (m_isInTunnel == false)
			{
				m_lockedMovementDirection = direction;
			}
		}
	};
}
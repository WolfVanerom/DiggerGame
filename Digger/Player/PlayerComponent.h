#pragma once
#include "Component.h"
#include "Observer.h"
#include <memory>
#include "LevelManager.h"

namespace dae
{
	class ProjectileComponent;
	class TextureComponent;
	class PlayerComponent final : public Component, public subject
	{
	private:
		int m_health{ 5 };
		int m_score{ 0 };
		int m_playerNumber{};

		float m_PlayerDeathAnimationTimer{ 0.f };

		int m_previousCellX{ -1 };
		int m_previousCellY{ -1 };
		float m_previousWorldX{ 0.f };
		float m_previousWorldY{ 0.f };
		float m_tunnelProgress{ 0.f };
		bool m_isInTunnel{ false };
		bool m_hasPreviousCell{ false };
		bool m_goldBagPickup{ false };
		bool m_isLocked{ false };
		bool m_isPlayingDeathAnimation{ false };
		TunnelDirection m_previousMovementDirection{ TunnelDirection::none };
		TunnelDirection m_lockedMovementDirection{ TunnelDirection::none };

		glm::vec3 m_startingPosition{ 0.f, 0.f, 0.f };

		LevelManager& m_levelManager{ LevelManager::GetInstance() };
		ProjectileComponent* m_pProjectileComponent{ nullptr };
		TextureComponent* m_pTextureComponent{ nullptr };

		void ChangePlayerTextureDirection(TunnelDirection direction);
	public:
		PlayerComponent(GameObject* pOwner, TextureComponent* pTextureComponent);
		~PlayerComponent() override;
		PlayerComponent(const PlayerComponent& other) = delete;
		PlayerComponent(PlayerComponent&& other) = delete;
		PlayerComponent& operator=(const PlayerComponent& other) = delete;
		PlayerComponent& operator=(PlayerComponent&& other) = delete;

		void Update(float deltaTime) override;

		void SetToStartingPosition();
		void SwitchLockControls();
		bool IsLocked() const;

		void ShootProjectile(TunnelDirection direction);

		void SubtractHealth(int amount);
		void PlayDeathAnimation();

		int GetHealth() const { return m_health; }
		int GetScore() const { return m_score; }
		int GetPlayerNumber() const { return m_playerNumber; }

		bool IsPlayerInCell(int cellX, int cellY) const;

		TunnelDirection GetLockedMovementDirection() const { return m_lockedMovementDirection; }

		bool CanMoveInDirection(TunnelDirection direction) const
		{
			return m_lockedMovementDirection == TunnelDirection::none or m_lockedMovementDirection == direction;
		}
		void SetLockedMovementDirection(TunnelDirection direction) {
			if (m_isInTunnel == false)
			{
				m_lockedMovementDirection = direction;
			}
		}
	};
}
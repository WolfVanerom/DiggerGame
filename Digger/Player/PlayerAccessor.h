#pragma once
#include <Singleton.h>
#include <vector>
#include "PlayerDamageComponent.h"


namespace dae
{
	class PlayerComponent;
	class PlayerAccessor final
	{
	public:
		PlayerComponent* GetPlayer(size_t index) const;
		std::vector<PlayerComponent*> GetPlayers() const;
		void AddPlayer(PlayerComponent* pPlayer);
		void RemovePlayer(PlayerComponent* pPlayer);
		int GetNextPlayerNumber();

		void SwitchLockAllPlayerControls();
		void SetToStartingPositionAllPlayers();
		void RestorePlayerHealthAllPlayers();
		void ReleasePlayerInvulnerabilityAllPlayers();
		void AddPlayerScore();
		void AddPlayerHealth();
	private:
		std::vector<PlayerComponent*> m_players;
		std::vector<PlayerDamageComponent*> m_playerDamageComponents;
		int m_nextPlayerNumber{ 1 };
	};
}
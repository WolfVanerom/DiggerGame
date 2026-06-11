#pragma once
#include <Singleton.h>
#include <vector>

namespace dae
{
	class PlayerComponent;
	class PlayerAccessor final
	{
	public:
		PlayerComponent* GetPlayer(size_t index) const;
		std::vector<PlayerComponent*> GetPlayers() const;
		void AddPlayer(PlayerComponent* pPlayer);
		void SwitchLockAllPlayerControls();
		void SetToStartingPositionAllPlayers();
		int GetNextPlayerNumber();
	private:
		std::vector<PlayerComponent*> m_players;
		int m_nextPlayerNumber{ 1 };
	};
}
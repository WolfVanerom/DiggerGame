#pragma once
#include <Singleton.h>
#include <vector>

namespace dae
{
	class PlayerComponent;
	class PlayerAccessor final : public Singleton<PlayerAccessor>
	{
	public:
		PlayerComponent* GetPlayer(size_t index) const;
		std::vector<PlayerComponent*> GetPlayers() const;
		void AddPlayer(PlayerComponent* pPlayer);
	private:
		std::vector<PlayerComponent*> m_players;
	};
}
#include "PlayerAccessor.h"
#include "PlayerComponent.h"
#include <vector>

dae::PlayerComponent* dae::PlayerAccessor::GetPlayer(size_t index) const
{
	if (index >= m_players.size())
	{
		return nullptr;
	}
	return m_players[index];
}

std::vector<dae::PlayerComponent*> dae::PlayerAccessor::GetPlayers() const
{
	return m_players;
}

void dae::PlayerAccessor::AddPlayer(PlayerComponent* pPlayer)
{
	m_players.push_back(pPlayer);
}
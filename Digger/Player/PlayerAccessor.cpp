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

void dae::PlayerAccessor::SwitchLockAllPlayerControls()
{
	for (auto* player : m_players)
	{
		player->SwitchLockControls();
	}
}

void dae::PlayerAccessor::SetToStartingPositionAllPlayers()
{
	for (auto* player : m_players)
	{
		player->SetToStartingPosition();
	}
}

int dae::PlayerAccessor::GetNextPlayerNumber()
{
	return m_nextPlayerNumber++;
}

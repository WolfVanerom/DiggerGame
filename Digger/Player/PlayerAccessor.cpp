#include "PlayerAccessor.h"
#include "PlayerComponent.h"
#include <ServiceLocator.h>
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
	PlayerDamageComponent* damageComponent = static_cast<PlayerDamageComponent*>(pPlayer->GetParent()->getComponent(typeid(PlayerDamageComponent)));
	m_playerDamageComponents.push_back(damageComponent);
}

void dae::PlayerAccessor::RemovePlayer(PlayerComponent* pPlayer)
{
	m_players.erase(std::remove(m_players.begin(), m_players.end(), pPlayer), m_players.end());
	PlayerDamageComponent* damageComponent = static_cast<PlayerDamageComponent*>(pPlayer->GetParent()->getComponent(typeid(PlayerDamageComponent)));
	m_playerDamageComponents.erase(std::remove(m_playerDamageComponents.begin(), m_playerDamageComponents.end(), damageComponent), m_playerDamageComponents.end());
	pPlayer->GetParent()->MarkForDeletion();
}

int dae::PlayerAccessor::GetNextPlayerNumber()
{
	return m_nextPlayerNumber++;
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

void dae::PlayerAccessor::RestorePlayerHealthAllPlayers()
{
	for (auto* player : m_players)
	{
		player->SetHealth(player->GetMaxHealth());
	}
}

void dae::PlayerAccessor::ReleasePlayerInvulnerabilityAllPlayers()
{
	for (auto* damageComponent : m_playerDamageComponents)
	{
		damageComponent->ReleaseInvulnerability();
	}
}

void dae::PlayerAccessor::AddPlayerScore()
{
	for (auto* player : m_players)
	{
		dae::serviceLocator::GetGameDataManager().AddCurrentScore(player->GetScore());
	}
}

void dae::PlayerAccessor::AddPlayerHealth()
{
	for (auto* player : m_players)
	{
		player->SetHealth(player->GetHealth() + 1);
	}
}
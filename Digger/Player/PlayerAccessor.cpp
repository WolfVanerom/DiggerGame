#include "PlayerAccessor.h"
#include "PlayerComponent.h"

dae::PlayerComponent* dae::PlayerAccessor::GetPlayer() const
{
	return m_pPlayer;
}

void dae::PlayerAccessor::SetPlayer(PlayerComponent* pPlayer)
{
	m_pPlayer = pPlayer;
}
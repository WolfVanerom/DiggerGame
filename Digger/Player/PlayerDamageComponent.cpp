#include "PlayerDamageComponent.h"
#include "PlayerComponent.h"
#include "HitBoxComponent.h"
#include "GoldComponent.h"
#include "GameObject.h"

namespace dae
{
	PlayerDamageComponent::PlayerDamageComponent(GameObject* owner, PlayerComponent* player, HitBoxComponent* hitbox)
		: Component(owner), m_player(player), m_hitbox(hitbox)
	{
		m_currentGameMode = serviceLocator::GetGameDataManager().GetGameMode();
		m_playerNumber = player->GetPlayerNumber();
	}

	void PlayerDamageComponent::Update(float deltatime)
	{
		if (m_isInvulnerable)
		{
			return;
		}

		for (auto* other : HitBoxComponent::CheckAll())
		{
			if (other == nullptr || other == m_hitbox) {
				continue;
			}
			if (!m_hitbox->Overlaps(*other)) {
				continue;
			}

			if((m_currentGameMode == GameMode::Versus and m_playerNumber == 1) or (m_currentGameMode == GameMode::SinglePlayer) or (m_currentGameMode == GameMode::Coop))
			{
				if (other->GetLayer() == HitboxLayer::Enemy)
				{
					m_player->SubtractHealth(1);
					m_isInvulnerable = true;
					return;
				}
			}

			if (other->GetLayer() == HitboxLayer::Item)
			{
				auto* gold = static_cast<GoldComponent*>(other->GetParent()->getComponent(typeid(GoldComponent)));
				if (gold && gold->IsDangerous())
				{
					m_player->SubtractHealth(1);
					m_isInvulnerable = true;
					return;
				}
			}
		}
	}
	void PlayerDamageComponent::ReleaseInvulnerability()
	{
		m_isInvulnerable = false;
	}
}
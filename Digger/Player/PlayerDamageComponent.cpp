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
	}

	void PlayerDamageComponent::Update(float deltatime)
	{
		if (m_invulnrebilityTimer > 0.f)
		{
			m_invulnrebilityTimer -= deltatime;
			return;
		}

		for (auto* other : HitBoxComponent::GetAll())
		{
			if (other == nullptr || other == m_hitbox) continue;
			if (!m_hitbox->Overlaps(*other)) continue;

			if (other->GetLayer() == HitboxLayer::Enemy)
			{
				m_player->SubtractHealth(1);
				m_invulnrebilityTimer = 0.75f;
				return;
			}

			if (other->GetLayer() == HitboxLayer::Item)
			{
				auto* gold = static_cast<GoldComponent*>(other->GetParent()->getComponent(typeid(GoldComponent)));
				if (gold && gold->IsDangerous())
				{
					m_player->SubtractHealth(1);
					m_invulnrebilityTimer = 0.75f;
					return;
				}
			}
		}
	}
}
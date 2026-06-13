#include "ScoreComponent.h"
#include <string>
#include "PlayerComponent.h"
#include "TextComponent.h"
#include <ServiceLocator.h>

namespace dae
{
    ScoreComponent::ScoreComponent(GameObject* pOwner, PlayerComponent* playerComponent, TextComponent* textComponent)
        : Component{ pOwner }
        , m_playerComponent{ playerComponent }
        , m_textComponent{ textComponent }
    {
        auto score = m_playerComponent->GetScore();
        if (m_textComponent && m_playerComponent)
        {
            m_textComponent->SetText("#Score = " + std::to_string(score));
        }

        if(score >= 20000)
        {
			serviceLocator::GetPlayerAccessor().AddPlayerHealth();
        }
    }

    void ScoreComponent::Update(float)
    {
    }

    void ScoreComponent::OnNotify(Event event, GameObject*)
    {
        if (event != Event::ScoreChanged)
            return;

        if (m_textComponent && m_playerComponent)
        {
            auto score = m_playerComponent->GetScore();
            m_textComponent->SetText("#Score = " + std::to_string(score));
        }
    }
}

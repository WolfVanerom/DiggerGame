#include "ScoreComponent.h"

#include <string>

#include "PlayerComponent.h"
#include "TextComponent.h"

namespace dae
{
    ScoreComponent::ScoreComponent(GameObject* pOwner, PlayerComponent* playerComponent, TextComponent* textComponent)
        : Component{ pOwner }
        , m_playerComponent{ playerComponent }
        , m_textComponent{ textComponent }
    {
        if (m_textComponent && m_playerComponent)
        {
            m_textComponent->SetText("#Score = " + std::to_string(m_playerComponent->GetScore()));
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
            m_textComponent->SetText("#Score = " + std::to_string(m_playerComponent->GetScore()));
        }
    }
}

#include "LifeComponent.h"

#include <string>

#include "PlayerComponent.h"
#include "TextComponent.h"

namespace dae
{
    LifeComponent::LifeComponent(GameObject* pOwner, PlayerComponent* playerComponent, TextComponent* textComponent)
        : Component{ pOwner }
        , m_playerComponent{ playerComponent }
        , m_textComponent{ textComponent }
    {
        if (m_textComponent && m_playerComponent)
        {
            m_textComponent->SetText("#Lives = " + std::to_string(m_playerComponent->GetHealth()));
        }
    }

    void LifeComponent::Update(float) {

    }

    void LifeComponent::OnNotify(Event event, GameObject*)
    {
        if (event != Event::RemainingLivesChanged)
            return;

        if (m_textComponent && m_playerComponent)
        {
            m_textComponent->SetText("#Lives = " + std::to_string(m_playerComponent->GetHealth()));
        }
    }
}

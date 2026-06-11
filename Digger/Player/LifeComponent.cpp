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
    }

    void LifeComponent::Update(float) {

    }

    void LifeComponent::OnNotify(Event event, GameObject*)
    {

        if (event == Event::RemainingLivesChanged)
        {
            m_enemySpawnManager.PauseEnemies();
			m_enemySpawnManager.SwitchPauseSpawning();
            m_playerAccessor.SwitchLockAllPlayerControls();
			m_soundSystem.playSound(4, 1.f, false);
            m_playerComponent->PlayDeathAnimation();

            if (m_textComponent && m_playerComponent)
            {
                m_textComponent->SetText("#Lives = " + std::to_string(m_playerComponent->GetHealth()));
            }
		}
        else if (event == Event::PlayerDeathAnimationFinished)
        {
			m_enemySpawnManager.ClearEnemies();
            m_enemySpawnManager.SwitchPauseSpawning();
			m_playerAccessor.SetToStartingPositionAllPlayers();
			m_playerAccessor.SwitchLockAllPlayerControls();
        }
        else if (event == Event::PlayerDied)
        {
			// Handle player death logic, such as checking for game over or resetting the level
        }
    }
}

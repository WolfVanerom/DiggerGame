#include "LifeComponent.h"
#include <string>
#include "PlayerComponent.h"
#include "TextComponent.h"
#include "SceneManager.h"
#include <LevelManager.h>
#include <InputManager.h>

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

        if (event == Event::RemainingLivesChanged and !m_isHandlingDeath)
        {
			m_isHandlingDeath = true;
            m_enemySpawnManager.PauseEnemies();
			m_enemySpawnManager.SwitchPauseSpawning();
            m_playerAccessor.SwitchLockAllPlayerControls();
			m_soundSystem.pauseAllSounds(false);
			m_soundSystem.playSound(4, 0.3f, false);
            m_playerComponent->PlayDeathAnimation();

            if (m_textComponent && m_playerComponent)
            {
                m_textComponent->SetText("#Lives = " + std::to_string(m_playerComponent->GetHealth()));
            }
		}
        else if (event == Event::PlayerDeathAnimationFinished and m_isHandlingDeath)
        {
			m_enemySpawnManager.ClearEnemies();
            m_enemySpawnManager.SwitchPauseSpawning();
			m_playerAccessor.SetToStartingPositionAllPlayers();
			m_soundSystem.playSound(2, 0.25f, true);
			m_playerAccessor.SwitchLockAllPlayerControls();
			m_playerAccessor.ReleasePlayerInvulnerabilityAllPlayers();
			m_isHandlingDeath = false;
        }
        else if (event == Event::PlayerDied)
        {
			m_soundSystem.pauseAllSounds(false);
            m_playerAccessor.AddPlayerScore();
			m_playerAccessor.SetToStartingPositionAllPlayers();
			m_enemySpawnManager.ClearEnemies();
			if (m_gameDataManager.GetGameMode() == GameMode::Coop or m_gameDataManager.GetGameMode() == GameMode::Versus)
            {
                dae::InputManager::GetInstance().RemoveCommandsForGameObject(m_playerComponent->GetParent());
				m_playerAccessor.RemovePlayer(m_playerComponent);
            }
			dae::SceneManager::GetInstance().SetActiveScene(m_gameDataManager.GetScoreSaveScene());
			dae::InputManager::GetInstance().ChangeMenuCommandContext(1);
            m_isHandlingDeath = false;
        }
    }
}

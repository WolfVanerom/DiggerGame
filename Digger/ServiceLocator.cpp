#include "ServiceLocator.h"

std::unique_ptr<dae::soundSystem> dae::serviceLocator::m_soundSystemInstance = nullptr;
std::unique_ptr<dae::PlayerAccessor> dae::serviceLocator::m_playerAccessorInstance = nullptr;
std::unique_ptr<dae::EnemySpawnManager> dae::serviceLocator::m_enemySpawnManagerInstance = nullptr;

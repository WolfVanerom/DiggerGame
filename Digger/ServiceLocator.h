#pragma once
#include <memory>
#include "SoundSystem.h"
#include <PlayerAccessor.h>
#include <Enemy/EnemySpawnManager.h>

namespace dae {
	class serviceLocator final
	{
		static std::unique_ptr<soundSystem> m_soundSystemInstance;
		static std::unique_ptr<PlayerAccessor> m_playerAccessorInstance;
		static std::unique_ptr<EnemySpawnManager> m_enemySpawnManagerInstance;
	public:
		static soundSystem& GetSoundSystem()
		{
			return *m_soundSystemInstance;
		}
		static void RegisterSoundSystem(std::unique_ptr<soundSystem> service)
		{
			m_soundSystemInstance = std::move(service);
		}
		static PlayerAccessor& GetPlayerAccessor()
		{
			return *m_playerAccessorInstance;
		}
		static void RegisterPlayerAccessor(std::unique_ptr<PlayerAccessor> service)
		{
			m_playerAccessorInstance = std::move(service);
		}
		static void RegisterEnemySpawnManager(std::unique_ptr<EnemySpawnManager> service)
		{
			m_enemySpawnManagerInstance = std::move(service);
		}
		static EnemySpawnManager& GetEnemySpawnManager()
		{
			return *m_enemySpawnManagerInstance;
		}
	};
}
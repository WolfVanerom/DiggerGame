#pragma once
#include <memory>
#include "SoundSystem.h"
#include <PlayerAccessor.h>
#include <Enemy/EnemySpawnManager.h>
#include <GameDataManager.h>
#include <GamePadSystem.h>

namespace dae {
	class serviceLocator final
	{
		static std::unique_ptr<soundSystem> m_soundSystemInstance;
		static std::unique_ptr<PlayerAccessor> m_playerAccessorInstance;
		static std::unique_ptr<EnemySpawnManager> m_enemySpawnManagerInstance;
		static std::unique_ptr<GameDataManager> m_gameDataManagerInstance;
		static std::unique_ptr<GamePadSystem> m_gamePadSystemInstance;
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
		static void RegisterGameDataManager(std::unique_ptr<GameDataManager> service)
		{
			m_gameDataManagerInstance = std::move(service);
		}
		static GameDataManager& GetGameDataManager()
		{
			return *m_gameDataManagerInstance;
		}
		static void RegisterGamePadSystem(std::unique_ptr<GamePadSystem> service)
		{
			m_gamePadSystemInstance = std::move(service);
		}
		static GamePadSystem& GetGamePadSystem()
		{
			return *m_gamePadSystemInstance;
		}
	};
}
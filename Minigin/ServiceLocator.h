#pragma once
#include <memory>
#include "SoundSystem.h"

namespace dae {
	class serviceLocator final
	{
		static std::unique_ptr<soundSystem> m_soundSystemInstance;
	public:
		static soundSystem& GetSoundSystem()
		{
			return *m_soundSystemInstance;
		}
		static void RegisterSoundSystem(std::unique_ptr<soundSystem> service)
		{
			m_soundSystemInstance = std::move(service);
		}
	};
}
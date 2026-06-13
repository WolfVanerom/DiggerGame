#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include "GamePadSystem.h"

namespace dae
{
	class SdlGamePadSystem final : public GamePadSystem
	{
	public:
		SdlGamePadSystem() = default;
		~SdlGamePadSystem() = default;
		SdlGamePadSystem(const SdlGamePadSystem& other) = delete;
		SdlGamePadSystem(SdlGamePadSystem&& other) noexcept = delete;
		SdlGamePadSystem& operator=(const SdlGamePadSystem& other) = delete;
		SdlGamePadSystem& operator=(SdlGamePadSystem&& other) noexcept = delete;
		void Initialize() override;
		int GetPlayerIdFromIndex(int index) const;
	private:
		SDL_JoystickID* m_Ids{ nullptr };
		std::vector<SDL_Gamepad*> m_Gamepads;
	};
}
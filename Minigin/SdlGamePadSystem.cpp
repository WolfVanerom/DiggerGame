#include "SdlGamePadSystem.h"
#include <SDL3/SDL.h>

namespace dae {
	void SdlGamePadSystem::Initialize()
	{
		if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
			SDL_Log("Failed to initialize gamepads");
			return;
		}

		int count = 0;

		m_Ids = SDL_GetGamepads(&count);
		m_Gamepads.resize(count);
		for (int i = 0; i < count; i++) {
			m_Gamepads[i] = nullptr;
		}

		for (int i = 0; i < count; i++) {
			SDL_Gamepad* gamepd = SDL_OpenGamepad(m_Ids[i]);
			if (m_Gamepads[i] == nullptr) {
				m_Gamepads[i] = gamepd;
				SDL_Log("Opened gamepad %i with ID %u", i, m_Ids[i]);
			}
		}

		if (!m_Gamepads[0]) {
			SDL_Log("Couldn't open gamepad 0");
			return;
		}
	}

	int SdlGamePadSystem::GetPlayerIdFromIndex(size_t index) const
	{
		if (index < 0 || static_cast<size_t>(index) >= m_Gamepads.size() || m_Gamepads[index] == nullptr) {
			return -1;
		}
		return m_Ids[index];
	}
}
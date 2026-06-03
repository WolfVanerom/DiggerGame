#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include "GameObject.h"

bool dae::InputManager::ProcessInput()
{
	SDL_Event e;


	while (SDL_PollEvent(&e)) {

		if (e.type == SDL_EVENT_QUIT) {
			return false;
		}
		if (e.type == SDL_EVENT_KEY_DOWN) {
			if (m_commands.find(e.key.key) != m_commands.end()) {
				m_commands[e.key.key]->Execute();
			}
			if (m_menuCommands.find(e.key.key) != m_menuCommands.end()) {
				m_menuCommands[e.key.key]->Execute();
			}
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {

		}

		ImGui_ImplSDL3_ProcessEvent(&e);
	}

	return true;
}
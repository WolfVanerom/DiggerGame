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
			if (m_activeMenuCommands->find(e.key.key) != m_activeMenuCommands->end()) {
				m_activeMenuCommands->at(e.key.key)->Execute();
			}
		}
		if (e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
			SDL_Log("Gamepad button %d pressed", e.gbutton.button);
			m_heldControllerButtons.insert(e.gbutton.button);
			if (m_commandsController.find(e.gbutton.button) != m_commandsController.end()) {
				m_commandsController[e.gbutton.button]->Execute();
			}
			if (m_activeMenuCommandsController->find(e.gbutton.button) != m_activeMenuCommandsController->end()) {
				m_activeMenuCommandsController->at(e.gbutton.button)->Execute();
			}
		}
		if (e.type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
			SDL_Log("Gamepad button %d released", e.gbutton.button);
			m_heldControllerButtons.erase(e.gbutton.button);
		}
		ImGui_ImplSDL3_ProcessEvent(&e);
	}

	for (int button : m_heldControllerButtons) {
		m_ButtonRepeatTimer += 0.5f;
		if(m_ButtonRepeatTimer >= m_ButtonRepeatDelay)
		{
			m_ButtonRepeatTimer = 0;
			if (m_commandsController.find(button) != m_commandsController.end()) {
				m_commandsController[button]->Execute();
			}
		}
	}

	return true;
}

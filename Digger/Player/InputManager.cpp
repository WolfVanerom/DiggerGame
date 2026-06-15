#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include "GameObject.h"

dae::GameObject* dae::GameObjectCommand::GetGameObject() const
{
	return m_gameObject;
}

void dae::moveCommand::Execute()
{
	if (m_playerComponent == nullptr)
	{
		return;
	}

	if (m_playerComponent->IsLocked())
	{
		return;
	}

	if (m_offsetX < 0.f)
	{
		m_playerComponent->SetLockedMovementDirection(TunnelDirection::left);
	}
	else if (m_offsetX > 0.f)
	{
		m_playerComponent->SetLockedMovementDirection(TunnelDirection::right);
	}
	else if (m_offsetY < 0.f)
	{
		m_playerComponent->SetLockedMovementDirection(TunnelDirection::up);
	}
	else if (m_offsetY > 0.f)
	{
		m_playerComponent->SetLockedMovementDirection(TunnelDirection::down);
	}

	float offsetX = m_offsetX;
	float offsetY = m_offsetY;

	switch (m_playerComponent->GetLockedMovementDirection())
	{
	case TunnelDirection::left:
		offsetX = -10.f;
		break;
	case TunnelDirection::right:
		offsetX = 10.f;
		break;
	case TunnelDirection::up:
		offsetY = -10.f;
		break;
	case TunnelDirection::down:
		offsetY = 10.f;
		break;
	case TunnelDirection::none:
		break;
	}

	auto pos = GetGameObject()->GetWorldPosition();
	auto& gameDataManager = serviceLocator::GetGameDataManager();
	auto posX = std::clamp(pos.x + offsetX, 0.f, gameDataManager.GetWindowSize().x - m_playerComponent->GetTextureWidth());
	auto posY = std::clamp(pos.y + offsetY, 0.f, gameDataManager.GetWindowSize().y - m_playerComponent->GetTextureHeight());
	GetGameObject()->SetPosition(posX, posY);
}

void dae::shootCommand::Execute()
{
	if (m_playerComponent != nullptr) {
		m_playerComponent->ShootProjectile(m_playerComponent->GetLockedMovementDirection());
	}
}

void dae::MenuMoveCommand::Execute()
{
	if (m_pMenuComponent != nullptr) {
		if (m_offsetY < 0.f)
		{
			m_pMenuComponent->MoveUp();
		}
		else if (m_offsetY > 0.f)
		{
			m_pMenuComponent->MoveDown();
		}
		else if (m_offsetX < 0.f)
		{
			m_pMenuComponent->MoveLeft();
		}
		else if (m_offsetX > 0.f)
		{
			m_pMenuComponent->MoveRight();
		}
	}
}

void dae::MenuSelectCommand::Execute()
{
	if (m_pMenuComponent != nullptr) {
		m_pMenuComponent->ClickSelectedItem();
	}
}

void dae::SwitchMuteSoundCommand::Execute()
{
	if (soundSystem.getIsMuted())
	{
		soundSystem.resumeAllSounds();
	}
	else
	{
		soundSystem.pauseAllSounds(true);
	}
}

void dae::SkipLevelCommand::Execute()
{
	m_levelManager.QueueLevelLoad("Data/levelData/" + std::to_string(m_levelManager.GetCurrentLevelIndex()) + ".txt", m_levelManager.GetCurrentScene());
	m_levelManager.IncreaseCurrentLevelIndex();
}

void dae::InputManager::ChangeMenuCommandContextKeyboard(int menuContext)
{
	switch (menuContext)
	{
	case 0:
		m_activeMenuCommands = &m_startMenuCommands;
		break;
	case 1:
		m_activeMenuCommands = &m_ScoreSaveMenuCommands;
		break;
	case 2:
		m_activeMenuCommands = &m_HighScoreMenuCommands;
		break;
	default:
		break;
	}
}

void dae::InputManager::ChangeMenuCommandContextController(int menuContext) {
	switch (menuContext)
	{
	case 0:
		m_activeMenuCommandsController = &m_startMenuCommandsController;
		break;
	case 1:
		m_activeMenuCommandsController = &m_ScoreSaveMenuCommandsController;
		break;
	case 2:
		m_activeMenuCommandsController = &m_HighScoreMenuCommandsController;
		break;
	default:
		break;
	}
}

void dae::InputManager::InitializeControlerIds() {
	auto& gamePadSystem = dae::serviceLocator::GetGamePadSystem();
	m_Player1ControllerId = static_cast<SDL_JoystickID>(gamePadSystem.GetPlayerIdFromIndex(0));
	m_Player2ControllerId = static_cast<SDL_JoystickID>(gamePadSystem.GetPlayerIdFromIndex(1));
}

void dae::InputManager::SwitchIsInMenuContext()
{
	m_IsInMenuContext = !m_IsInMenuContext;
}

bool dae::InputManager::GetIsInMenuContext() const
{
	return m_IsInMenuContext;
}

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
			SDL_Log("which=%u button=%d", e.gbutton.which, e.gbutton.button);
			if (e.gbutton.which == m_Player1ControllerId) {
				m_heldControllerButtons[e.gbutton.which].insert(e.gbutton.button);
				if (m_commandsControllerPlayer1.find(e.gbutton.button) != m_commandsControllerPlayer1.end()) {
					m_commandsControllerPlayer1[e.gbutton.button]->Execute();
				}
			}
			else if (e.gbutton.which == m_Player2ControllerId) {
				m_heldControllerButtons[e.gbutton.which].insert(e.gbutton.button);
				if (m_commandsControllerPlayer2.find(e.gbutton.button) != m_commandsControllerPlayer2.end()) {
					m_commandsControllerPlayer2[e.gbutton.button]->Execute();
				}
			}
			if (!m_IsInMenuContext) {
				return true;
			}
			if (m_activeMenuCommandsController->find(e.gbutton.button) != m_activeMenuCommandsController->end()) {
				m_activeMenuCommandsController->at(e.gbutton.button)->Execute();
			}
		}
		if (e.type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
			SDL_Log("Gamepad button %d released", e.gbutton.button);
			m_heldControllerButtons[e.gbutton.which].erase(e.gbutton.button);
		}
		ImGui_ImplSDL3_ProcessEvent(&e);
	}

	for (const auto& pair : m_heldControllerButtons) {
		for (int button : pair.second) {
			if (pair.first == m_Player1ControllerId) {
				m_ButtonRepeatTimerPlayer1 += 0.5f;
				if (m_ButtonRepeatTimerPlayer1 >= m_ButtonRepeatDelay)
				{
					m_ButtonRepeatTimerPlayer1 = 0;
					if (m_commandsControllerPlayer1.find(button) != m_commandsControllerPlayer1.end()) {
						m_commandsControllerPlayer1[button]->Execute();
					}
				}
			}
			else if (pair.first == m_Player2ControllerId) {
				m_ButtonRepeatTimerPlayer2 += 0.5f;
				if (m_ButtonRepeatTimerPlayer2 >= m_ButtonRepeatDelay)
				{
					m_ButtonRepeatTimerPlayer2 = 0;
					if (m_commandsControllerPlayer2.find(button) != m_commandsControllerPlayer2.end()) {
						m_commandsControllerPlayer2[button]->Execute();
					}
				}
			}
		}
	}
	return true;
}

void dae::InputManager::AddCommand(int key, std::unique_ptr<Command> command)
{
	m_commands[key] = std::move(command);
}

void dae::InputManager::AddCommandControllerPlayer1(int key, std::unique_ptr<Command> command)
{
	m_commandsControllerPlayer1[key] = std::move(command);
}

void dae::InputManager::AddCommandControllerPlayer2(int key, std::unique_ptr<Command> command)
{
	m_commandsControllerPlayer2[key] = std::move(command);
}

void dae::InputManager::AddMenuCommand(int key, std::unique_ptr<Command> command)
{
	m_activeMenuCommands->insert({ key, std::move(command) });
}

void dae::InputManager::AddMenuCommandController(int key, std::unique_ptr<Command> command)
{
	m_activeMenuCommandsController->insert({ key, std::move(command) });
}

void dae::InputManager::RemoveCommand(int key)
{
	m_commands.erase(key);
}

void dae::InputManager::RemoveCommandControllerPlayer1(int key)
{
	m_commandsControllerPlayer1.erase(key);
}

void dae::InputManager::RemoveCommandControllerPlayer2(int key)
{
	m_commandsControllerPlayer2.erase(key);
}	

void dae::InputManager::RemoveMenuCommand(int key)
{
	m_activeMenuCommands->erase(key);
}

void dae::InputManager::RemoveMenuCommandController(int key)
{
	m_activeMenuCommandsController->erase(key);
}

void dae::InputManager::RemoveCommandsForGameObject(dae::GameObject* gameObject) {
	for (auto it = m_commands.begin(); it != m_commands.end(); ) {
		GameObjectCommand* gameObjectCommand = dynamic_cast<GameObjectCommand*>(it->second.get());
		if (gameObjectCommand && gameObjectCommand->m_gameObject == gameObject) {
			it = m_commands.erase(it);
		}
		else {
			++it;
		}
	}
	for (auto it = m_commandsControllerPlayer1.begin(); it != m_commandsControllerPlayer1.end(); ) {
		GameObjectCommand* gameObjectCommand = dynamic_cast<GameObjectCommand*>(it->second.get());
		if (gameObjectCommand && gameObjectCommand->m_gameObject == gameObject) {
			it = m_commandsControllerPlayer1.erase(it);
		}
		else {
			++it;
		}
	}
	for (auto it = m_commandsControllerPlayer2.begin(); it != m_commandsControllerPlayer2.end(); ) {
		GameObjectCommand* gameObjectCommand = dynamic_cast<GameObjectCommand*>(it->second.get());
		if (gameObjectCommand && gameObjectCommand->m_gameObject == gameObject) {
			it = m_commandsControllerPlayer2.erase(it);
		}
		else {
			++it;
		}
	}
}

void dae::InputManager::RemoveMenuCommandsForGameObject(dae::GameObject* gameObject) {
	for (auto it = m_activeMenuCommands->begin(); it != m_activeMenuCommands->end(); ) {
		GameObjectCommand* gameObjectCommand = dynamic_cast<GameObjectCommand*>(it->second.get());
		if (gameObjectCommand && gameObjectCommand->m_gameObject == gameObject) {
			it = m_activeMenuCommands->erase(it);
		}
		else {
			++it;
		}
	}
	for (auto it = m_activeMenuCommandsController->begin(); it != m_activeMenuCommandsController->end(); ) {
		GameObjectCommand* gameObjectCommand = dynamic_cast<GameObjectCommand*>(it->second.get());
		if (gameObjectCommand && gameObjectCommand->m_gameObject == gameObject) {
			it = m_activeMenuCommandsController->erase(it);
		}
		else {
			++it;
		}
	}
}



#pragma once
#include "Singleton.h"
#include "GameObject.h"
#include "PlayerComponent.h"
#include <cmath>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <MenuComponent.h>
#include <ServiceLocator.h>
#include "SdlSoundSystem.h"
#include <SDL3/SDL.h>

namespace dae
{
	class Command {
	public:
		virtual ~Command() = default;
		virtual void Execute() = 0;
	};

	class GameObjectCommand : public dae::Command {
	public:
		dae::GameObject* m_gameObject;
	protected:
		dae::GameObject* GetGameObject() const;
		GameObjectCommand(dae::GameObject* gameObject) : m_gameObject(gameObject) {}
		virtual ~GameObjectCommand() = default;
	};

	class moveCommand : public GameObjectCommand {
	private:
		dae::PlayerComponent* m_playerComponent;
		float m_offsetX;
		float m_offsetY;
	public:
		moveCommand(dae::GameObject* gameObject, PlayerComponent* playerComponent, float offsetX, float offsetY)
			: GameObjectCommand(gameObject), m_playerComponent(playerComponent), m_offsetX(offsetX), m_offsetY(offsetY) {
		}

		void Execute() override;
	};

	class shootCommand : public GameObjectCommand {
	private:
		dae::PlayerComponent* m_playerComponent;
	public:
		shootCommand(dae::GameObject* gameObject, PlayerComponent* playerComponent)
			: GameObjectCommand(gameObject), m_playerComponent(playerComponent) {}

		void Execute() override;
	};

	class MenuMoveCommand : public GameObjectCommand {
		MenuComponent* m_pMenuComponent;
		float m_offsetX;
		float m_offsetY;
	public:
		MenuMoveCommand(dae::GameObject* gameObject, dae::MenuComponent* pMenuComponent, float offsetX, float offsetY)
			: GameObjectCommand(gameObject), m_pMenuComponent(pMenuComponent), m_offsetX(offsetX), m_offsetY(offsetY) {
		}
		void Execute() override;
	};

	class MenuSelectCommand : public GameObjectCommand {
		MenuComponent* m_pMenuComponent;
	public:
		explicit MenuSelectCommand(dae::GameObject* gameObject, dae::MenuComponent* pMenuComponent) : GameObjectCommand(gameObject), m_pMenuComponent(pMenuComponent)
		{
		}
		void Execute() override;
	};

	class SwitchMuteSoundCommand : public Command {
		soundSystem& soundSystem = dae::serviceLocator::GetSoundSystem();
	public:
		SwitchMuteSoundCommand() {}
		void Execute() override;
	};

	class SkipLevelCommand : public Command {
		LevelManager& m_levelManager = LevelManager::GetInstance();
	public:
		SkipLevelCommand() {}
		void Execute() override;
	};

	class InputManager final : public Singleton<InputManager>
	{
	private:
		std::unordered_map<int, std::unique_ptr<Command>> m_commands;
		std::unordered_map<int, std::unique_ptr<Command>>* m_activeMenuCommands;
		std::unordered_map<int, std::unique_ptr<Command>> m_startMenuCommands {};
		std::unordered_map<int, std::unique_ptr<Command>> m_ScoreSaveMenuCommands {};
		std::unordered_map<int, std::unique_ptr<Command>> m_HighScoreMenuCommands {};
		std::unordered_map<int, std::unique_ptr<Command>> m_commandsControllerPlayer1;
		std::unordered_map<int, std::unique_ptr<Command>> m_commandsControllerPlayer2;
		std::unordered_map<int, std::unique_ptr<Command>>* m_activeMenuCommandsController;
		std::unordered_map<int, std::unique_ptr<Command>> m_startMenuCommandsController{};
		std::unordered_map<int, std::unique_ptr<Command>> m_ScoreSaveMenuCommandsController{};
		std::unordered_map<int, std::unique_ptr<Command>> m_HighScoreMenuCommandsController{};
		std::unordered_map<SDL_JoystickID, std::unordered_set<int>> m_heldControllerButtons;

		SDL_JoystickID m_Player1ControllerId{};
		SDL_JoystickID m_Player2ControllerId{};

		bool m_IsInMenuContext{ true };
		float m_TimePassedSinceLastProjectile{ 0 };
		float m_ButtonRepeatDelay{ 2 };
		float m_ButtonRepeatTimerPlayer1{ 0 };
		float m_ButtonRepeatTimerPlayer2{ 0 };

		void ChangeMenuCommandContextKeyboard(int menuContext);

		void ChangeMenuCommandContextController(int menuContext);
	public:
		void InitializeControlerIds();
		void SwitchIsInMenuContext();
		bool GetIsInMenuContext() const;
		bool ProcessInput();

		void AddCommand(int key, std::unique_ptr<Command> command);
		void AddCommandControllerPlayer1(int key, std::unique_ptr<Command> command);
		void AddCommandControllerPlayer2(int key, std::unique_ptr<Command> command);

		void ChangeMenuCommandContext(int menuContext) {
			ChangeMenuCommandContextKeyboard(menuContext);
			ChangeMenuCommandContextController(menuContext);
		}

		void AddMenuCommand(int key, std::unique_ptr<Command> command);
		void AddMenuCommandController(int key, std::unique_ptr<Command> command);

		void RemoveCommand(int key);
		void RemoveCommandControllerPlayer1(int key);
		void RemoveCommandControllerPlayer2(int key);
		void RemoveMenuCommand(int key);
		void RemoveMenuCommandController(int key);
		void RemoveCommandsForGameObject(dae::GameObject* gameObject);
		void RemoveMenuCommandsForGameObject(dae::GameObject* gameObject);
	};
}

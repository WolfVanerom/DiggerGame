#pragma once
#include "Singleton.h"
#include "GameObject.h"
#include "PlayerComponent.h"
#include <cmath>
#include <memory>
#include <unordered_map>

namespace dae
{
	class Command {
	public:
		virtual ~Command() = default;
		virtual void Execute() = 0;
	};

	class CommandWithValue : public Command {
	private:
		std::unique_ptr<Command> m_command;
	public:
		CommandWithValue(std::unique_ptr<Command> command, [[maybe_unused]] float value)
			: m_command(std::move(command)) {}

		void Execute() override {
			m_command->Execute();
		}
	};

	class GameObjectCommand : public dae::Command {
	public:
		dae::GameObject* m_gameObject;
	protected:
		dae::GameObject* GetGameObject() const { return m_gameObject; }
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

		void Execute() override {
			if (m_playerComponent == nullptr)
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
			GetGameObject()->SetPosition(pos.x + offsetX, pos.y + offsetY);
		}
	};

	class damageCommand : public GameObjectCommand {
		dae::PlayerComponent* m_playerComponent;
	public:
		explicit damageCommand(dae::GameObject* gameObject, dae::PlayerComponent* playerComponent) : GameObjectCommand(gameObject), m_playerComponent(playerComponent)
		{
		}
		void Execute() override {
			if (m_playerComponent != nullptr)
			{
				static_cast<PlayerComponent*>(m_playerComponent)->SubtractHealth(1);
			}
		}
	};

	class InputManager final : public Singleton<InputManager>
	{
	private:
		std::unordered_map<int, std::unique_ptr<Command>> m_commands;
	public:
		bool ProcessInput();
		void AddCommand(int key, std::unique_ptr<Command> command) {
			m_commands[key] = std::move(command);
		}
		void RemoveCommand(int key) {
			m_commands.erase(key);
		}
		void RemoveCommandsForGameObject(dae::GameObject* gameObject) {
			for (auto it = m_commands.begin(); it != m_commands.end(); ) {
				GameObjectCommand* gameObjectCommand = dynamic_cast<GameObjectCommand*>(it->second.get());
				if (gameObjectCommand && gameObjectCommand->m_gameObject == gameObject) {
					it = m_commands.erase(it);
				}
				else {
					++it;
				}
			}
		}
	};

}

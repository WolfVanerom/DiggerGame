#pragma once
#include <glm/vec3.hpp>

namespace dae {
	class EnemyComponent;
	class PlayerComponent;
	class EnemyState
	{
	public:
		EnemyComponent* m_pEnemyComponent{ nullptr };
		PlayerComponent* m_pPlayerComponent{ nullptr };
		virtual void OnEnter() = 0;
		virtual void OnExit() = 0;
		virtual void Update(float deltaTime) = 0;
	protected:
		glm::vec3 DistanceToDirection();
		glm::vec3 DistanceToDirectionDigging();
		float DirectionToMoveOffset(bool calculateX);
		bool FindNextCellInDirection(float offsetX, float offsetY);
	};

	class NobbinState final : public EnemyState
	{
		void OnEnter() override;
		void OnExit() override;
		void Update(float deltaTime) override;
	};

	class HobbinState final : public EnemyState
	{
		void OnEnter() override;
		void OnExit() override;
		void Update(float deltaTime) override;
	private:
		int m_amountOfCellsTraveled{ 0 };
	};

	class PauseState final : public EnemyState
	{
		void OnEnter() override;
		void OnExit() override;
		void Update(float deltaTime) override;
	};
}
#include "EnemyState.h"
#include <GameObject.h>
#include "EnemyComponent.h"
#include "PlayerComponent.h"
#include "LevelManager.h"
#include <cmath>

namespace dae
{
	glm::vec3 EnemyState::DistanceToDirection()
	{
		m_pEnemyComponent->m_Position = m_pEnemyComponent->m_parent->GetWorldPosition();
		m_pEnemyComponent->m_PlayerPosition = m_pPlayerComponent->GetParent()->GetWorldPosition();
		auto differenceX = m_pEnemyComponent->m_PlayerPosition.x - m_pEnemyComponent->m_Position.x;
		auto differenceY = m_pEnemyComponent->m_PlayerPosition.y - m_pEnemyComponent->m_Position.y;

		float preferredOffsetX = 0.f;
		float preferredOffsetY = 0.f;
		float secondaryOffsetX = 0.f;
		float secondaryOffsetY = 0.f;

		if (std::abs(differenceX) >= std::abs(differenceY)) {
			preferredOffsetX = DirectionToMoveOffset(true);
			secondaryOffsetY = DirectionToMoveOffset(false);
		}
		else {
			preferredOffsetY = DirectionToMoveOffset(false);
			secondaryOffsetX = DirectionToMoveOffset(true);
		}

		if ((preferredOffsetX != 0.f or preferredOffsetY != 0.f) and FindNextCellInDirection(preferredOffsetX, preferredOffsetY)) {
			return glm::vec3(preferredOffsetX, preferredOffsetY, 0.f);
		}

		if ((secondaryOffsetX != 0.f or secondaryOffsetY != 0.f) and FindNextCellInDirection(secondaryOffsetX, secondaryOffsetY)) {
			return glm::vec3(secondaryOffsetX, secondaryOffsetY, 0.f);
		}

		if (m_pEnemyComponent->m_Direction.x != 0.f or m_pEnemyComponent->m_Direction.y != 0.f) {
			if (FindNextCellInDirection(m_pEnemyComponent->m_Direction.x, m_pEnemyComponent->m_Direction.y)) {
				return m_pEnemyComponent->m_Direction;
			}
		}

		return glm::vec3(0.f, 0.f, 0.f);
	}

	float EnemyState::DirectionToMoveOffset(bool calculateX)
	{
		if (calculateX) {
			if (m_pEnemyComponent->m_Position.x < m_pEnemyComponent->m_PlayerPosition.x)
			{
				return 1.f;
			}
			else if (m_pEnemyComponent->m_Position.x > m_pEnemyComponent->m_PlayerPosition.x)
			{
				return -1.f;
			}
			else
			{
				return 0.f;
			}
		}
		else {
			if (m_pEnemyComponent->m_Position.y < m_pEnemyComponent->m_PlayerPosition.y)
			{
				return 1.f;
			}
			else if (m_pEnemyComponent->m_Position.y > m_pEnemyComponent->m_PlayerPosition.y)
			{
				return -1.f;
			}
			else
			{
				return 0.f;
			}
		}
	}

	bool EnemyState::FindNextCellInDirection(float offsetX, float offsetY)
	{
		if (offsetX == 0.f and offsetY == 0.f) {
			return false;
		}

		offsetX *= LevelManager::m_tileWidth;
		offsetY *= LevelManager::m_tileHeight;

		auto currentCellX = static_cast<int>(std::round(m_pEnemyComponent->m_Position.x / LevelManager::m_tileWidth));
		auto currentCellY = static_cast<int>(std::round(m_pEnemyComponent->m_Position.y / LevelManager::m_tileHeight));
		auto targetCellX = static_cast<int>(std::round((m_pEnemyComponent->m_Position.x + offsetX) / LevelManager::m_tileWidth));
		auto targetCellY = static_cast<int>(std::round((m_pEnemyComponent->m_Position.y + offsetY) / LevelManager::m_tileHeight));

		int searchAttempts{};
		while (currentCellX == targetCellX and currentCellY == targetCellY and searchAttempts < 8) {
			offsetX *= 2.f;
			offsetY *= 2.f;
			targetCellX = static_cast<int>(std::round((m_pEnemyComponent->m_Position.x + offsetX) / LevelManager::m_tileWidth));
			targetCellY = static_cast<int>(std::round((m_pEnemyComponent->m_Position.y + offsetY) / LevelManager::m_tileHeight));
			++searchAttempts;
		}

		if (currentCellX == targetCellX and currentCellY == targetCellY) {
			return false;
		}

		auto cellType = m_pEnemyComponent->m_levelManager.GetCell(targetCellX, targetCellY);
		return cellType == LevelObjectType::horizontalTunnel or cellType == LevelObjectType::tunnelEnd or cellType == LevelObjectType::verticalTunnel;
	}

	void NobbinState::OnEnter()
	{
		m_pEnemyComponent->m_pTextureComponent->SetTexture("media/cnob1.png");
	}
	void NobbinState::OnExit()
	{
	}
	void NobbinState::Update(float deltaTime)
	{
		m_pEnemyComponent->m_Position = m_pEnemyComponent->m_parent->GetWorldPosition();

		const auto currentCellX = static_cast<int>(std::round(m_pEnemyComponent->m_Position.x / LevelManager::m_tileWidth));
		const auto currentCellY = static_cast<int>(std::round(m_pEnemyComponent->m_Position.y / LevelManager::m_tileHeight));
		const float cellOriginX = static_cast<float>(currentCellX) * LevelManager::m_tileWidth;
		const float cellOriginY = static_cast<float>(currentCellY) * LevelManager::m_tileHeight;
		const float centerTolerance = 2.0f;

		if (m_pEnemyComponent->m_Direction.x == 0.f and m_pEnemyComponent->m_Direction.y == 0.f) {
			m_pEnemyComponent->m_Direction = DistanceToDirection();
		}

		bool canMoveCurrentDirection = FindNextCellInDirection(m_pEnemyComponent->m_Direction.x, m_pEnemyComponent->m_Direction.y);
		bool hasPerpendicularOption = false;
		const bool horizontalMove = m_pEnemyComponent->m_Direction.x != 0.f;
		const bool verticalMove = m_pEnemyComponent->m_Direction.y != 0.f;

		if (horizontalMove) {
			hasPerpendicularOption = FindNextCellInDirection(0.f, 1.f) or FindNextCellInDirection(0.f, -1.f);
		}
		else if (verticalMove) {
			hasPerpendicularOption = FindNextCellInDirection(1.f, 0.f) or FindNextCellInDirection(-1.f, 0.f);
		}

		if (!canMoveCurrentDirection or (hasPerpendicularOption)) {
			auto newDirection = DistanceToDirection();
			if (newDirection.x != 0.f or newDirection.y != 0.f) {
				m_pEnemyComponent->m_Direction = newDirection;
				canMoveCurrentDirection = FindNextCellInDirection(m_pEnemyComponent->m_Direction.x, m_pEnemyComponent->m_Direction.y);
			}
		}

		if (!canMoveCurrentDirection and (m_pEnemyComponent->m_Direction.x != 0.f or m_pEnemyComponent->m_Direction.y != 0.f)) {
			m_pEnemyComponent->m_Direction = glm::vec3(-m_pEnemyComponent->m_Direction.x, -m_pEnemyComponent->m_Direction.y, 0.f);
			canMoveCurrentDirection = FindNextCellInDirection(m_pEnemyComponent->m_Direction.x, m_pEnemyComponent->m_Direction.y);
		}

		if (canMoveCurrentDirection) {
			constexpr float enemyMoveSpeed{ 50.f };
			const float moveStep = enemyMoveSpeed * deltaTime;
			float nextX = m_pEnemyComponent->m_Position.x + (m_pEnemyComponent->m_Direction.x * moveStep);
			float nextY = m_pEnemyComponent->m_Position.y + (m_pEnemyComponent->m_Direction.y * moveStep);

			if (horizontalMove) {
				nextY = cellOriginY;
			}
			else if (verticalMove) {
				nextX = cellOriginX;
			}

			m_pEnemyComponent->m_parent->SetPosition(nextX, nextY);
		}

		//for (auto* enemy : m_pEnemyComponent->m_enemySpawnManager.GetEnemies()) {
		//	auto enemyCellX = static_cast<int>(std::round(enemy->m_Position.x / LevelManager::m_tileWidth));
		//	auto enemyCellY = static_cast<int>(std::round(enemy->m_Position.y / LevelManager::m_tileHeight));
		//	if (currentCellX == enemyCellX and currentCellY == enemyCellY) {
		//		m_pEnemyComponent->SetState(m_pEnemyComponent->m_pHobbinState);
		//		m_pEnemyComponent->m_pCurrentState->OnEnter();
		//		return;
		//	}
		//}
	}

	void HobbinState::OnEnter()
	{
		/*m_pEnemyComponent->m_pTextureComponent->SetTexture("media/clhob1.png");*/
	}
	void HobbinState::OnExit()
	{
	}
	void HobbinState::Update(float)
	{
		//auto moveToOffset = DistanceToDirection();
		//m_pEnemyComponent->m_Position = m_pEnemyComponent->m_parent->GetWorldPosition();
		//auto canMove = FindNextCellInDirection(moveToOffset.x, moveToOffset.y);

		//if (canMove) {
		//	m_pEnemyComponent->m_parent->SetPosition(m_pEnemyComponent->m_Position.x + moveToOffset.x, m_pEnemyComponent->m_Position.y + moveToOffset.y);
		//}

		//auto currentCellX = static_cast<int>(std::round(m_pEnemyComponent->m_Position.x / LevelManager::m_tileWidth));
		//auto currentCellY = static_cast<int>(std::round(m_pEnemyComponent->m_Position.y / LevelManager::m_tileHeight));

		//for (auto* enemy : m_pEnemyComponent->m_enemySpawnManager.GetEnemies()) {
		//	auto enemyCellX = static_cast<int>(std::round(enemy->m_Position.x / LevelManager::m_tileWidth));
		//	auto enemyCellY = static_cast<int>(std::round(enemy->m_Position.y / LevelManager::m_tileHeight));
		//	if (currentCellX == enemyCellX and currentCellY == enemyCellY) {
		//		m_pEnemyComponent->SetState(m_pEnemyComponent->m_pHobbinState);
		//		m_pEnemyComponent->m_pCurrentState->OnEnter();
		//		return;
		//	}
		//}
	}

	void PauseState::OnEnter()
	{
	}
	void PauseState::OnExit()
	{
	}
	void PauseState::Update(float)
	{
	}
}
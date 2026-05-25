#include "EnemyState.h"
#include <GameObject.h>
#include "EnemyComponent.h"
#include "PlayerComponent.h"
#include "LevelManager.h"

namespace dae
{
	glm::vec3 EnemyState::DistanceToDirection()
	{
		m_pEnemyComponent->m_Position = m_pEnemyComponent->m_parent->GetWorldPosition();
		m_pEnemyComponent->m_PlayerPosition = m_pPlayerComponent->GetParent()->GetWorldPosition();
		auto diffrenceX = m_pEnemyComponent->m_PlayerPosition.x - m_pEnemyComponent->m_Position.x;
		auto diffrenceY = m_pEnemyComponent->m_PlayerPosition.y - m_pEnemyComponent->m_Position.y;

		float xOffset = 0.f;
		float yOffset = 0.f;

		if (diffrenceX > diffrenceY) {
			xOffset = DirectionToMoveOffset(true);
		}
		else {
			yOffset = DirectionToMoveOffset(false);
		}
		
		return glm::vec3(xOffset, yOffset, 0.f);
	}

	float EnemyState::DirectionToMoveOffset(bool calculateX)
	{
		if (calculateX) {
			if (m_pEnemyComponent->m_Position.x < m_pEnemyComponent->m_PlayerPosition.x)
			{
				return 10.f;
			}
			else if (m_pEnemyComponent->m_Position.x > m_pEnemyComponent->m_PlayerPosition.x)
			{
				return -10.f;
			}
			else
			{
				return 0.f;
			}
		}
		else {
			if (m_pEnemyComponent->m_Position.y < m_pEnemyComponent->m_PlayerPosition.y)
			{
				return 10.f;
			}
			else if (m_pEnemyComponent->m_Position.y > m_pEnemyComponent->m_PlayerPosition.y)
			{
				return -10.f;
			}
			else
			{
				return 0.f;
			}
		}
	}

	bool EnemyState::FindNextCellInDirection(float offsetX, float offsetY)
	{
		auto currentCellX = static_cast<int>(std::round(m_pEnemyComponent->m_Position.x / LevelManager::m_tileWidth));
		auto currentCellY = static_cast<int>(std::round(m_pEnemyComponent->m_Position.y / LevelManager::m_tileHeight));
		auto targetCellX = static_cast<int>(std::round((m_pEnemyComponent->m_Position.x + offsetX) / LevelManager::m_tileWidth));
		auto targetCellY = static_cast<int>(std::round((m_pEnemyComponent->m_Position.y + offsetY) / LevelManager::m_tileHeight));
		if (currentCellX == targetCellX && currentCellY == targetCellY) {
			return FindNextCellInDirection(offsetX * 2.f, offsetY * 2.f);
		}
		auto cellType = m_pEnemyComponent->m_levelManager.GetCell(targetCellX, targetCellY);
		return cellType == LevelObjectType::horizontalTunnel || cellType == LevelObjectType::tunnelEnd || cellType == LevelObjectType::verticalTunnel;
	}

	void NobbinState::OnEnter()
	{
		m_pEnemyComponent->m_pTextureComponent->SetTexture("media/cnob1.png");
	}
	void NobbinState::OnExit()
	{
	}
	void NobbinState::Update(float)
	{
		auto moveToOffset = DistanceToDirection();
		m_pEnemyComponent->m_Position = m_pEnemyComponent->m_parent->GetWorldPosition();
		auto canMove = FindNextCellInDirection(moveToOffset.x, moveToOffset.y);

		if (canMove) {
			m_pEnemyComponent->m_parent->SetPosition(m_pEnemyComponent->m_Position.x + moveToOffset.x, m_pEnemyComponent->m_Position.y + moveToOffset.y);
		}

		auto currentCellX = static_cast<int>(std::round(m_pEnemyComponent->m_Position.x / LevelManager::m_tileWidth));
		auto currentCellY = static_cast<int>(std::round(m_pEnemyComponent->m_Position.y / LevelManager::m_tileHeight));

		for (auto* enemy : m_pEnemyComponent->m_enemySpawnManager.GetEnemies()) {
			auto enemyCellX = static_cast<int>(std::round(enemy->m_Position.x / LevelManager::m_tileWidth));
			auto enemyCellY = static_cast<int>(std::round(enemy->m_Position.y / LevelManager::m_tileHeight));
			if (currentCellX == enemyCellX && currentCellY == enemyCellY) {
				m_pEnemyComponent->SetState(m_pEnemyComponent->m_pHobbinState);
				m_pEnemyComponent->m_pCurrentState->OnEnter();
				return;
			}
		}
	}

	void HobbinState::OnEnter()
	{
		m_pEnemyComponent->m_pTextureComponent->SetTexture("media/clhob1.png");
	}
	void HobbinState::OnExit()
	{
	}
	void HobbinState::Update(float)
	{
		auto moveToOffset = DistanceToDirection();
		m_pEnemyComponent->m_Position = m_pEnemyComponent->m_parent->GetWorldPosition();

		m_pEnemyComponent->m_parent->SetPosition(m_pEnemyComponent->m_Position.x + moveToOffset.x, m_pEnemyComponent->m_Position.y + moveToOffset.y);

		auto currentCellX = static_cast<int>(std::round(m_pEnemyComponent->m_Position.x / LevelManager::m_tileWidth));
		auto currentCellY = static_cast<int>(std::round(m_pEnemyComponent->m_Position.y / LevelManager::m_tileHeight));

		if (moveToOffset.x != 0.f) {
			m_pEnemyComponent->m_levelManager.SetCell(currentCellX, currentCellY, LevelObjectType::horizontalTunnel);
		}
		else if (moveToOffset.y != 0.f) {
			m_pEnemyComponent->m_levelManager.SetCell(currentCellX, currentCellY, LevelObjectType::horizontalTunnel);
		}
	}
}
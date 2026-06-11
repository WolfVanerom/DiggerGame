#include "ProjectileComponent.h"
#include <Level/LevelManager.h>
#include <GameObject.h>
#include <SDL3/SDL.h>

namespace dae
{

	ProjectileComponent::ProjectileComponent(GameObject* pOwner, TunnelDirection direction)
		: Component(pOwner), m_Direction(direction)
	{
		m_pLevelManager = &LevelManager::GetInstance();
		m_MoveOffset = DirectionToMoveOffset(m_Direction);
	}

	void ProjectileComponent::Update(float deltaTime)
	{
		if (m_Direction == TunnelDirection::left or m_Direction == TunnelDirection::right)
		{
			int nextCellX = static_cast<int>(std::round((m_parent->GetWorldPosition().x + m_MoveOffset.x) / LevelManager::m_tileWidth));
			int cellY = static_cast<int>(std::round(m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));
			auto cell = m_pLevelManager->GetCell(nextCellX, cellY);
			if (cell == LevelObjectType::none or cell == LevelObjectType::empty)
			{
				m_parent->MarkForDeletion();
				return;
			}
		}
		else if (m_Direction == TunnelDirection::up or m_Direction == TunnelDirection::down)
		{
			int cellX = static_cast<int>(std::round(m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
			int nextCellY = static_cast<int>(std::round(m_parent->GetWorldPosition().y + m_MoveOffset.y / LevelManager::m_tileHeight));
			auto cell = m_pLevelManager->GetCell(cellX, nextCellY);
			if (cell == LevelObjectType::none or cell == LevelObjectType::empty)
			{
				m_parent->MarkForDeletion();
				return;
			}
		}
		m_parent->SetPosition(m_parent->GetWorldPosition() + m_MoveOffset * deltaTime);
	}

	glm::vec3 ProjectileComponent::DirectionToMoveOffset(TunnelDirection direction)
	{
		switch (direction)
		{
		case TunnelDirection::up:
			return glm::vec3{ 0.f, -m_MoveOffsetDistance, 0.f };
		case TunnelDirection::down:
			return glm::vec3{ 0.f, m_MoveOffsetDistance, 0.f };
		case TunnelDirection::left:
			return glm::vec3{ -m_MoveOffsetDistance, 0.f, 0.f };
		case TunnelDirection::right:
			return glm::vec3{ m_MoveOffsetDistance, 0.f, 0.f };
		default:
			return glm::vec3{ 0.f, 0.f, 0.f };
		}
	}
}
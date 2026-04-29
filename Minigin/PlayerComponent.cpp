#include "PlayerComponent.h"
#include "Renderer.h"
#include <algorithm>

namespace
{
	using dae::TunnelDirection;
	using dae::LevelObjectType;

	TunnelDirection GetDirectionFromDelta(float deltaX, float deltaY, TunnelDirection fallbackDirection)
	{
		const float absDeltaX = deltaX < 0.f ? -deltaX : deltaX;
		const float absDeltaY = deltaY < 0.f ? -deltaY : deltaY;
		if (absDeltaX == 0.f && absDeltaY == 0.f)
		{
			return fallbackDirection;
		}

		if (absDeltaX >= absDeltaY)
		{
			return deltaX >= 0.f ? TunnelDirection::right : TunnelDirection::left;
		}

		return deltaY >= 0.f ? TunnelDirection::down : TunnelDirection::up;
	}

	dae::LevelObjectType GetTrailType(TunnelDirection direction)
	{
		if (direction == TunnelDirection::left || direction == TunnelDirection::right)
		{
			return LevelObjectType::horizontalTunnel;
		}

		if (direction == TunnelDirection::up || direction == TunnelDirection::down)
		{
			return LevelObjectType::verticalTunnel;
		}

		return LevelObjectType::none;
	}
}

dae::PlayerComponent::PlayerComponent(GameObject* pOwner)
	:Component(pOwner)
{
}

dae::PlayerComponent::~PlayerComponent()
{
}

void dae::PlayerComponent::Update(float deltaTime)
{
	(void)deltaTime;
	const auto& worldPos = m_parent->GetWorldPosition();
	const float worldX = worldPos.x;
	const float worldY = worldPos.y;
	const int cellX = static_cast<int>(worldPos.x / LevelManager::m_tileWidth);
	const int cellY = static_cast<int>(worldPos.y / LevelManager::m_tileHeight);
	const bool hasMovedToNewCell = m_hasPreviousCell && (cellX != m_previousCellX || cellY != m_previousCellY);
	const float deltaX = worldX - m_previousWorldX;
	const float deltaY = worldY - m_previousWorldY;
	const auto movementDirection = GetDirectionFromDelta(deltaX, deltaY, m_previousMovementDirection);
	const auto trailType = GetTrailType(movementDirection);

	if (m_levelManager.GetCell(cellX, cellY) == LevelObjectType::emerald)
	{
		m_score += 100;
		m_levelManager.SetCell(cellX, cellY, LevelObjectType::empty);

		Notify(Event::ScoreChanged, m_parent);
	}

	if (!m_hasPreviousCell)
	{
		m_hasPreviousCell = true;
		m_previousCellX = cellX;
		m_previousCellY = cellY;
		m_previousWorldX = worldX;
		m_previousWorldY = worldY;
		m_previousMovementDirection = movementDirection;
		m_levelManager.ClearTunnelPreview();
		return;
	}

	if (trailType != LevelObjectType::none)
	{
		const float cellOriginX = static_cast<float>(cellX) * LevelManager::m_tileWidth;
		const float cellOriginY = static_cast<float>(cellY) * LevelManager::m_tileHeight;
		float progress = 0.f;

		if (movementDirection == TunnelDirection::left)
		{
			m_parent->SetPosition(worldX, cellOriginY + (LevelManager::m_tileHeight * 0.15f));
			progress = (cellOriginX + LevelManager::m_tileWidth - worldX) / LevelManager::m_tileWidth;
		}
		else if (movementDirection == TunnelDirection::right)
		{
			m_parent->SetPosition(worldX, cellOriginY + (LevelManager::m_tileHeight * 0.15f));
			progress = (worldX - cellOriginX) / LevelManager::m_tileWidth;
		}
		else if (movementDirection == TunnelDirection::up)
		{
			m_parent->SetPosition(cellOriginX + (LevelManager::m_tileWidth * 0.20f), worldY);
			progress = (cellOriginY + LevelManager::m_tileHeight - worldY) / LevelManager::m_tileHeight;
		}
		else if (movementDirection == TunnelDirection::down)
		{
			m_parent->SetPosition(cellOriginX + (LevelManager::m_tileWidth * 0.20f), worldY);
			progress = (worldY - cellOriginY) / LevelManager::m_tileHeight;
		}

		m_levelManager.SetTunnelPreview(cellX, cellY, trailType, movementDirection, progress);
	}
	else
	{
		m_levelManager.ClearTunnelPreview();
	}

	if (hasMovedToNewCell)
	{
		m_levelManager.SetCell(m_previousCellX, m_previousCellY, trailType);

		m_previousCellX = cellX;
		m_previousCellY = cellY;
	}

	m_previousWorldX = worldX;
	m_previousWorldY = worldY;
	m_previousMovementDirection = movementDirection;
}

void dae::PlayerComponent::SubtractHealth(int amount)
{
	m_health -= amount;
	m_health = std::max(0, m_health);
	Notify(Event::RemainingLivesChanged, m_parent);

	if (m_health <= 0)
	{
		Notify(Event::PlayerDied, m_parent);
	}
}

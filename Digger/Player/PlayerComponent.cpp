#include "PlayerComponent.h"
#include "Renderer.h"
#include <algorithm>
#include "ServiceLocator.h"
#include "GoldComponent.h"
#include <TextureComponent.h>

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
	if (direction == TunnelDirection::left or direction == TunnelDirection::right)
	{
		return LevelObjectType::horizontalTunnel;
	}

	if (direction == TunnelDirection::up or direction == TunnelDirection::down)
	{
		return LevelObjectType::verticalTunnel;
	}

	return LevelObjectType::none;
}

void dae::PlayerComponent::ChangePlayerTextureDirection(TunnelDirection direction)
{
	switch (direction)
	{
	case TunnelDirection::up:
		m_pTextureComponent->SetRotation(90);
		m_pTextureComponent->FlipHorizontal(false);
		m_pTextureComponent->FlipVertical(true);
		break;
	case TunnelDirection::down:
		m_pTextureComponent->SetRotation(90);
		m_pTextureComponent->FlipHorizontal(true);
		m_pTextureComponent->FlipVertical(true);
		break;
	case TunnelDirection::left:
		m_pTextureComponent->SetRotation(0);
		m_pTextureComponent->FlipVertical(false);
		m_pTextureComponent->FlipHorizontal(false);
		break;
	case TunnelDirection::right:
		m_pTextureComponent->SetRotation(0);
		m_pTextureComponent->FlipVertical(false);
		m_pTextureComponent->FlipHorizontal(true);
		break;
	default:
		break;
	}
}

dae::PlayerComponent::PlayerComponent(GameObject* pOwner, TextureComponent* pTextureComponent)
	:Component(pOwner), m_pTextureComponent(pTextureComponent)
{
	m_playerNumber = serviceLocator::GetPlayerAccessor().GetNextPlayerNumber();
	m_startingPosition = m_parent->GetWorldPosition();
}

dae::PlayerComponent::~PlayerComponent()
{
}

void dae::PlayerComponent::Update(float deltaTime)
{
	if (m_isPlayingDeathAnimation)
	{
		m_PlayerDeathAnimationTimer += deltaTime;
		if (m_PlayerDeathAnimationTimer >= 6.f)
		{
			if (m_health <= 0)
			{
				Notify(Event::PlayerDied, m_parent);
			}
			else {
				m_pTextureComponent->SetTexture("media/cldig1.png");
				Notify(Event::PlayerDeathAnimationFinished, m_parent);
			}
			m_isPlayingDeathAnimation = false;
		}
		return;
	}

	const auto& worldPos = m_parent->GetWorldPosition();
	const float worldX = worldPos.x;
	const float worldY = worldPos.y;
	const float deltaX = worldX - m_previousWorldX;
	const float deltaY = worldY - m_previousWorldY;
	auto movementDirection = GetDirectionFromDelta(deltaX, deltaY, m_previousMovementDirection);
	const auto trailType = GetTrailType(movementDirection);

	float probeX = worldX;
	float probeY = worldY;
	if (movementDirection == TunnelDirection::right)
	{
		probeX += LevelManager::m_tileWidth * 0.60f;
		probeY += LevelManager::m_tileHeight * 0.35f;
	}
	else if (movementDirection == TunnelDirection::left)
	{
		probeY += LevelManager::m_tileHeight * 0.35f;
	}
	else if (movementDirection == TunnelDirection::down)
	{
		probeX += LevelManager::m_tileWidth * 0.30f;
		probeY += LevelManager::m_tileHeight * 0.70f;
	}
	else if (movementDirection == TunnelDirection::up)
	{
		probeX += LevelManager::m_tileWidth * 0.30f;
		probeY += LevelManager::m_tileHeight * 0.15f;
	}
	else
	{
		probeX += LevelManager::m_tileWidth * 0.30f;
		probeY += LevelManager::m_tileHeight * 0.35f;
	}

	m_currentCellX = static_cast<int>(probeX / LevelManager::m_tileWidth);
	m_currentCellY = static_cast<int>(probeY / LevelManager::m_tileHeight);

	if (m_levelManager.GetCell(m_currentCellX, m_currentCellY) == LevelObjectType::emerald)
	{
		m_score += 25;
		m_emeraldsCollected++;
		m_levelManager.SetCell(m_currentCellX, m_currentCellY, LevelObjectType::empty);

		if (m_emeraldsCollected == 8) {
			m_score += 250;
		}

		auto& soundSystem = dae::serviceLocator::GetSoundSystem();
		soundSystem.playSound(1, 0.5f, false);

		Notify(Event::ScoreChanged, m_parent);

		m_levelManager.CheckIfLevelCompleted();
	}
	else if(m_levelManager.GetCell(m_currentCellX, m_currentCellY) == LevelObjectType::bag)
	{
		auto* object = m_levelManager.GetEntityObjectFromCell(m_currentCellX, m_currentCellY);
		if (object == nullptr)
		{
			return;
		}

		GoldComponent& goldComponent = static_cast<GoldComponent&>(*object->getComponent(typeid(GoldComponent)));
		if(goldComponent.HasBroken() == false)
		{
			return;
		}

		//SDL_Log("Gold collected at cell (%d, %d)", m_currentCellX, m_currentCellY);
		m_score += 500;
		m_levelManager.SetCell(m_currentCellX, m_currentCellY, LevelObjectType::empty);

		auto& soundSystem = dae::serviceLocator::GetSoundSystem();
		soundSystem.playSound(1, 0.5f, false);

		Notify(Event::ScoreChanged, m_parent);
	}


	if (LevelManager::GetInstance().GetCell(m_currentCellX, m_currentCellY) == LevelObjectType::empty)
	{
		const float cellOriginX = static_cast<float>(m_currentCellX) * LevelManager::m_tileWidth;
		const float cellOriginY = static_cast<float>(m_currentCellY) * LevelManager::m_tileHeight;

		if (m_tunnelProgress > 0.f) {
			m_isInTunnel = true;
		}

		if (movementDirection == TunnelDirection::left)
		{
			m_parent->SetPosition(worldX, cellOriginY + (LevelManager::m_tileHeight * 0.15f));
			m_tunnelProgress = (cellOriginX + LevelManager::m_tileWidth - probeX) / LevelManager::m_tileWidth;
		}
		else if (movementDirection == TunnelDirection::right)
		{
			m_parent->SetPosition(worldX, cellOriginY + (LevelManager::m_tileHeight * 0.15f));
			m_tunnelProgress = (probeX - cellOriginX) / LevelManager::m_tileWidth;
		}
		else if (movementDirection == TunnelDirection::up)
		{
			m_parent->SetPosition(cellOriginX + (LevelManager::m_tileWidth * 0.15f), worldY);
			m_tunnelProgress = (cellOriginY + LevelManager::m_tileHeight - probeY) / LevelManager::m_tileHeight;
		}
		else if (movementDirection == TunnelDirection::down)
		{
			m_parent->SetPosition(cellOriginX + (LevelManager::m_tileWidth * 0.15f), worldY);
			m_tunnelProgress = (probeY - cellOriginY) / LevelManager::m_tileHeight;
		}

		m_levelManager.SetTunnelPreview(m_parent, m_currentCellX, m_currentCellY, trailType, movementDirection, m_tunnelProgress);

		//SDL_Log("Tunnel progress: %f", m_tunnelProgress);

		if (m_tunnelProgress >= 0.8f)
		{
			m_levelManager.SetCell(m_currentCellX, m_currentCellY, trailType);

			m_isInTunnel = false;

			m_tunnelProgress = 0.f;
		}
	}
	else
	{
		switch (movementDirection) {
		case TunnelDirection::up:
			ChangePlayerTextureDirection(TunnelDirection::up);
			break;
		case TunnelDirection::down:
			ChangePlayerTextureDirection(TunnelDirection::down);
			break;
		case TunnelDirection::left:
			ChangePlayerTextureDirection(TunnelDirection::left);
			break;
		case TunnelDirection::right:
			ChangePlayerTextureDirection(TunnelDirection::right);
			break;
		}
		m_levelManager.ClearTunnelPreview(m_parent);
		m_isInTunnel = false;
		m_tunnelProgress = 0.f;
	}

	const auto& finalWorldPos = m_parent->GetWorldPosition();
	m_previousWorldX = finalWorldPos.x;
	m_previousWorldY = finalWorldPos.y;
	m_previousMovementDirection = movementDirection;
}

void dae::PlayerComponent::SetToStartingPosition()
{
	m_parent->SetPosition(m_startingPosition);
}

void dae::PlayerComponent::SwitchLockControls()
{
	m_isLocked = !m_isLocked;
}

bool dae::PlayerComponent::IsLocked() const
{
	return m_isLocked;
}

void dae::PlayerComponent::ShootProjectile(TunnelDirection direction)
{
	m_levelManager.SpawnProjectileAt(m_currentCellX, m_currentCellY, direction);
}

void dae::PlayerComponent::SetHealth(int health)
{
	m_health = health;
}

void dae::PlayerComponent::SubtractHealth(int amount)
{
	m_health -= amount;
	m_health = std::max(0, m_health);
	Notify(Event::RemainingLivesChanged, m_parent);
}

void dae::PlayerComponent::PlayDeathAnimation()
{
	m_pTextureComponent->SetTexture("media/cgrave5.png");
	m_PlayerDeathAnimationTimer = 0.f;
	m_isPlayingDeathAnimation = true;
}

int dae::PlayerComponent::GetHealth() const
{
	return m_health;
}

int dae::PlayerComponent::GetMaxHealth() const
{
	return m_maxHealth;
}

int dae::PlayerComponent::GetScore() const
{
	return m_score;
}

void dae::PlayerComponent::AddScore(int score)
{
	m_score += score;
	Notify(Event::ScoreChanged, m_parent);
}

int dae::PlayerComponent::GetPlayerNumber() const
{
	return m_playerNumber;
}

float dae::PlayerComponent::GetTextureWidth() const
{
	return m_pTextureComponent->GetWidth();
}

float dae::PlayerComponent::GetTextureHeight() const
{
	return m_pTextureComponent->GetHeight();
}

bool dae::PlayerComponent::IsPlayerInCell(int cellX, int cellY) const
{
	const auto& worldPos = m_parent->GetWorldPosition();
	const int playerCellX = static_cast<int>(worldPos.x / LevelManager::m_tileWidth);
	const int playerCellY = static_cast<int>(worldPos.y / LevelManager::m_tileHeight);
	return playerCellX == cellX && playerCellY == cellY;
}

TunnelDirection dae::PlayerComponent::GetLockedMovementDirection()
{
	return m_lockedMovementDirection;
}

bool dae::PlayerComponent::CanMoveInDirection(TunnelDirection direction) const
{
	return m_lockedMovementDirection == TunnelDirection::none or m_lockedMovementDirection == direction;
}

void dae::PlayerComponent::SetLockedMovementDirection(TunnelDirection direction)
{
	if (m_isInTunnel == false)
	{
		m_lockedMovementDirection = direction;
	}
}
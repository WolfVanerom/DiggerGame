#include "GoldState.h"
#include "GoldComponent.h"
#include "LevelManager.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include <SDL3/SDL_log.h>
#include <PlayerComponent.h>
#include <vector>

namespace dae {
	void IdleState::OnEnter()
	{
		m_pGoldComponent->m_HasFallen = false;
		m_pGoldComponent->m_Broken = false;

		int cellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		int cellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));
		m_pGoldComponent->m_originalCellX = cellX;
		m_pGoldComponent->m_originalCellY = cellY;
	}
	void IdleState::OnExit()
	{
	}
	void IdleState::Update(float)
	{ 	
		int cellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		int cellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));
		int targetCellX = cellX;
		int targetCellY = cellY;
		
		auto cellBelow = m_pGoldComponent->m_levelManager.GetCell(cellX, cellY + 1);
		
		if (cellBelow == LevelObjectType::horizontalTunnel or cellBelow == LevelObjectType::tunnelEnd or cellBelow == LevelObjectType::verticalTunnel)
		{
			m_pGoldComponent->m_pCurrentState = m_pGoldComponent->m_pFallingState;
			m_pGoldComponent->m_pCurrentState->OnEnter();
		}

		auto players = m_pGoldComponent->m_playerAccessor.GetPlayers();
		for (auto player : players) {
			TunnelDirection playerDirection = player->GetLockedMovementDirection();

			auto playerXPos = player->GetParent()->GetWorldPosition().x;
			auto goldXPos = m_pGoldComponent->m_parent->GetWorldPosition().x;
			auto goldYPos = m_pGoldComponent->m_parent->GetWorldPosition().y;
			const float pushStep = m_pTextureComponent->GetWidth() * 0.1f;

			switch (playerDirection)
			{
			case TunnelDirection::left:
			{
				const float leftDistance = std::abs(playerXPos - (goldXPos + m_pTextureComponent->GetWidth()));
				if (std::round(player->IsPlayerInCell(cellX, cellY)) and leftDistance < m_pTextureComponent->GetWidth()) {
					m_pGoldComponent->m_parent->SetPosition(goldXPos - pushStep, goldYPos);
				}
				break;
			}
			case TunnelDirection::right:
				const float rightDistance = std::abs(playerXPos - goldXPos);
				if (std::round(player->IsPlayerInCell(cellX - 1, cellY)) and rightDistance < m_pTextureComponent->GetWidth()) {
					m_pGoldComponent->m_parent->SetPosition(goldXPos + pushStep, goldYPos);
				}
				break;
			}
		}

		const int newCellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		const int newCellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));
		if (newCellX != m_pGoldComponent->m_originalCellX || newCellY != m_pGoldComponent->m_originalCellY)
		{
			if (m_pGoldComponent->m_levelManager.GetCell(newCellX, newCellY) == LevelObjectType::emerald 
				or m_pGoldComponent->m_levelManager.GetCell(newCellX, newCellY) == LevelObjectType::gold 
				or m_pGoldComponent->m_levelManager.GetCell(newCellX, newCellY) == LevelObjectType::bag) 
			{
				m_pGoldComponent->m_levelManager.SetCell(newCellX, newCellY, LevelObjectType::empty);
				m_pGoldComponent->m_levelManager.CheckIfLevelCompleted();
			}
			m_pGoldComponent->m_levelManager.MoveEntityCell(m_pGoldComponent->m_originalCellX, m_pGoldComponent->m_originalCellY, newCellX, newCellY, LevelObjectType::bag, false);

			m_pGoldComponent->m_originalCellX = newCellX;
			m_pGoldComponent->m_originalCellY = newCellY;
		}
	}
	void IdleState::SetTextureComponent(TextureComponent* textureComponent)
	{
		m_pTextureComponent = textureComponent;
	}
	void FallingState::OnEnter()
	{
		m_pGoldComponent->m_HasFallen = true;
	}
	void FallingState::OnExit()
	{
	}
	void FallingState::Update(float)
	{ 	
		if (m_FallDelayTimer < m_FallDelay)
		{
			m_FallDelayTimer += 0.1f;
			return;
		}

		int cellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		int cellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));
		int cellYBelow = cellY + 1;
		
		auto cellBelow = m_pGoldComponent->m_levelManager.GetCell(cellX, cellYBelow);
		
		SDL_Log("Amount Fallen: %d, Cell Below: %d", m_AmountFallen, static_cast<int>(cellBelow));

		if (cellBelow == LevelObjectType::horizontalTunnel or cellBelow == LevelObjectType::tunnelEnd or cellBelow == LevelObjectType::verticalTunnel)
		{
			float newY = m_pGoldComponent->m_parent->GetWorldPosition().y + (LevelManager::m_tileHeight * 0.1f);
			m_pGoldComponent->m_parent->SetPosition(m_pGoldComponent->m_parent->GetWorldPosition().x, newY);
			if (cellY != m_PrevCellY) {
				m_AmountFallen++;
			}
			m_PrevCellY = cellY;
		}
		else {
			if (m_AmountFallen < 2) {
				LevelManager::GetInstance().MoveEntityCell(m_pGoldComponent->m_originalCellX, m_pGoldComponent->m_originalCellY, cellX, cellY , LevelObjectType::bag, false);
				m_pGoldComponent->m_originalCellX = cellX;
				m_pGoldComponent->m_originalCellY = cellY;
				m_pGoldComponent->m_parent->SetPosition(cellX * LevelManager::m_tileWidth, cellY * LevelManager::m_tileHeight);
				m_FallDelayTimer = 0.f;
				m_pGoldComponent->m_pCurrentState = m_pGoldComponent->m_pIdleState;
				m_pGoldComponent->m_pCurrentState->OnEnter();
			}
			else {
				m_pGoldComponent->m_pCurrentState = m_pGoldComponent->m_pBrokenState;
				m_pGoldComponent->m_pCurrentState->OnEnter();
			}
		}
	}

	void BrokenState::OnEnter()
	{
		m_pGoldComponent->m_Broken = true;
		m_pTextureComponent->SetTexture("media/cgold3.png");

		int cellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		int cellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));

		m_pGoldComponent->m_levelManager.MoveEntityCell(m_pGoldComponent->m_originalCellX, m_pGoldComponent->m_originalCellY, cellX, cellY, LevelObjectType::bag, false);
		m_pGoldComponent->m_originalCellX = cellX;
		m_pGoldComponent->m_originalCellY = cellY;
		m_pGoldComponent->m_parent->SetPosition(cellX * LevelManager::m_tileWidth, cellY * LevelManager::m_tileHeight);
	}
	void BrokenState::OnExit()
	{
	}
	void BrokenState::Update(float)
	{
	}
	void BrokenState::SetTextureComponent(TextureComponent* textureComponent)
	{
		m_pTextureComponent = textureComponent;
	}
}
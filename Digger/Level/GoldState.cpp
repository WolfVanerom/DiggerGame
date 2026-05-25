#include "GoldState.h"
#include "GoldComponent.h"
#include "LevelManager.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include <SDL3/SDL_log.h>

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
		
		auto cellBelow = m_pGoldComponent->m_levelManager.GetCell(cellX, cellY + 1);
		
		if (cellBelow == LevelObjectType::horizontalTunnel or cellBelow == LevelObjectType::tunnelEnd or cellBelow == LevelObjectType::verticalTunnel)
		{
			m_pGoldComponent->m_pCurrentState = m_pGoldComponent->m_pFallingState;
			m_pGoldComponent->m_pCurrentState->OnEnter();
		}
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
		int cellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		int cellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));
		int cellYBelow = cellY + 1;
		
		auto cellBelow = m_pGoldComponent->m_levelManager.GetCell(cellX, cellYBelow);
		
		SDL_Log("Amount Fallen: %d, Cell Below: %d", m_AmountFallen, static_cast<int>(cellBelow));

		if (cellBelow == LevelObjectType::none or cellBelow == LevelObjectType::empty)
		{
			if (m_AmountFallen < 2) {
				m_pGoldComponent->m_pCurrentState = m_pGoldComponent->m_pIdleState;
				m_pGoldComponent->m_pCurrentState->OnEnter();
			}
			else{
				m_pGoldComponent->m_pCurrentState = m_pGoldComponent->m_pBrokenState;
				m_pGoldComponent->m_pCurrentState->OnEnter();
			}
		}
		else if (cellBelow == LevelObjectType::horizontalTunnel or cellBelow == LevelObjectType::tunnelEnd or cellBelow == LevelObjectType::verticalTunnel)
		{
			float newY = m_pGoldComponent->m_parent->GetWorldPosition().y + (LevelManager::m_tileHeight * 0.1f);
			m_pGoldComponent->m_parent->SetPosition(m_pGoldComponent->m_parent->GetWorldPosition().x, newY);
			if (cellY != m_PrevCellY) {
				m_AmountFallen++;
			}
			m_PrevCellY = cellY;
		}
	}

	void BrokenState::OnEnter()
	{
		m_pGoldComponent->m_Broken = true;
		m_pTextureComponent->SetTexture("media/cgold3.png");

		int cellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		int cellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));

		m_pGoldComponent->m_levelManager.MoveEntityCell(m_pGoldComponent->m_originalCellX, m_pGoldComponent->m_originalCellY, cellX, cellY, LevelObjectType::bag, false);
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
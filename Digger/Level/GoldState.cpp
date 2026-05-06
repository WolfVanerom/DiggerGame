#include "GoldState.h"
#include "GoldComponent.h"
#include "LevelManager.h"
#include "GameObject.h"
#include "TextureComponent.h"

namespace dae {
	void GoldState::SetState()
	{
	}

	void IdleState::SetState()
	{
		m_pGoldComponent->m_HasFallen = false;
		m_pGoldComponent->m_Broken = false;

		int cellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		int cellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));
		m_pGoldComponent->m_originalCellX = cellX;
		m_pGoldComponent->m_originalCellY = cellY;

		m_pGoldComponent->SetState(this);
	}
	void IdleState::Update(float deltaTime)
	{
		(void)deltaTime;
		
		int cellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		int cellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));
		
		auto cellBelow = m_pGoldComponent->m_levelManager.GetCell(cellX, cellY + 1);
		
		if (cellBelow == LevelObjectType::horizontalTunnel || cellBelow == LevelObjectType::tunnelEnd || cellBelow == LevelObjectType::verticalTunnel)
		{
			m_pGoldComponent->m_pCurrentState = m_pGoldComponent->m_pFallingState;
			m_pGoldComponent->m_pCurrentState->SetState();
		}
	}

	void FallingState::SetState()
	{
		m_pGoldComponent->m_HasFallen = true;
		m_pGoldComponent->SetState(this);
	}
	void FallingState::Update(float deltaTime)
	{
		(void)deltaTime;
		
		int cellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		int cellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));
		int cellYBelow = cellY + 1;
		
		auto cellBelow = m_pGoldComponent->m_levelManager.GetCell(cellX, cellYBelow);
		
		if (cellBelow == LevelObjectType::none || cellBelow == LevelObjectType::empty)
		{
			m_pGoldComponent->m_pCurrentState = m_pGoldComponent->m_pBrokenState;
			m_pGoldComponent->m_pCurrentState->SetState();
		}
		else if (cellBelow == LevelObjectType::horizontalTunnel || cellBelow == LevelObjectType::tunnelEnd || cellBelow == LevelObjectType::verticalTunnel)
		{
			float newY = m_pGoldComponent->m_parent->GetWorldPosition().y + (LevelManager::m_tileHeight * 0.1f);
			m_pGoldComponent->m_parent->SetPosition(m_pGoldComponent->m_parent->GetWorldPosition().x, newY);
		}
	}

	void BrokenState::SetState()
	{
		m_pGoldComponent->m_Broken = true;
		m_pTextureComponent->SetTexture("media/cgold3.png");
		m_pGoldComponent->SetState(this);

		int cellX = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		int cellY = static_cast<int>(std::round(m_pGoldComponent->m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));

		if (m_pGoldComponent->m_originalCellX >= 0 && m_pGoldComponent->m_originalCellY >= 0)
		{
			m_pGoldComponent->m_levelManager.MoveEntityCell(m_pGoldComponent->m_originalCellX, m_pGoldComponent->m_originalCellY, cellX, cellY, LevelObjectType::bag);
			m_pGoldComponent->m_parent->SetPosition(cellX * LevelManager::m_tileWidth, cellY * LevelManager::m_tileHeight);
		}
	}
	void BrokenState::Update(float deltaTime)
	{
		(void)deltaTime;
	}
	void BrokenState::SetTextureComponent(TextureComponent* textureComponent)
	{
		m_pTextureComponent = textureComponent;
	}
}
#include "GoldComponent.h"
#include "GameObject.h"
#include "LevelManager.h"
#include "GoldState.h"
#include "TextureComponent.h"

namespace dae{
	GoldComponent::GoldComponent(GameObject* pOwner)
		: Component(pOwner)
		, m_pIdleState(new IdleState())
		, m_pFallingState(new FallingState())
		, m_pBrokenState(new BrokenState())
	{
		m_pIdleState->SetGoldComponent(this);
		m_pFallingState->SetGoldComponent(this);
		m_pBrokenState->SetGoldComponent(this);
		m_pIdleState->SetTextureComponent(static_cast<TextureComponent*>(m_parent->getComponent(typeid(TextureComponent))));
		m_pBrokenState->SetTextureComponent(static_cast<TextureComponent*>(m_parent->getComponent(typeid(TextureComponent))));
		m_pCurrentState = m_pIdleState;
		m_originalCellX = static_cast<int>(std::round(m_parent->GetWorldPosition().x / LevelManager::m_tileWidth));
		m_originalCellY = static_cast<int>(std::round(m_parent->GetWorldPosition().y / LevelManager::m_tileHeight));
	}

	void GoldComponent::Update(float deltaTime)
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Update(deltaTime);
		}
	}
}
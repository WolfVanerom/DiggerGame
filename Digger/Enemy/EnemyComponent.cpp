#include "EnemyComponent.h"
#include "EnemyState.h"
#include "PlayerComponent.h"

namespace dae
{
	EnemyComponent::EnemyComponent(GameObject* pOwner, PlayerComponent* pPlayerComponent, TextureComponent* pTextureComponent)
		: Component(pOwner)
		, m_pTextureComponent(pTextureComponent)
		, m_pNobbinState(new NobbinState())
		, m_pHobbinState(new HobbinState())
		, m_pPauseState(new PauseState())
		, m_pCurrentState(m_pNobbinState)
	{
		m_pNobbinState->m_pEnemyComponent = this;
		m_pNobbinState->m_pPlayerComponent = pPlayerComponent;
		m_pHobbinState->m_pEnemyComponent = this;
		m_pHobbinState->m_pPlayerComponent = pPlayerComponent;
		m_pPauseState->m_pEnemyComponent = this;
		m_pPauseState->m_pPlayerComponent = pPlayerComponent;
	}
	void EnemyComponent::Update(float deltaTime)
	{
		if (m_pCurrentState)
		{
			m_pCurrentState->Update(deltaTime);
		}
	}
	void EnemyComponent::SetState(EnemyState* newState)
	{
		m_pCurrentState = newState;
	}
	void EnemyComponent::SetStateFromType(EnemyStateType newStateType)
	{
		switch (newStateType)
		{
		case EnemyStateType::Nobbin:
			SetState(m_pNobbinState);
			break;
		case EnemyStateType::Hobbin:
			SetState(m_pHobbinState);
			break;
		case EnemyStateType::Pause:
			SetState(m_pPauseState);
			break;
		}
	}
}
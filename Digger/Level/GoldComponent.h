#pragma once
#include "Component.h"
#include "LevelManager.h"
#include "PlayerAccessor.h"
#include <ServiceLocator.h>
#include <GoldState.h>

namespace dae {
	class GameObject;
	class LevelManager;

	class GoldComponent final : public Component
	{
	public:
		GoldComponent(GameObject* pOwner);
		virtual ~GoldComponent() = default;
		GoldComponent(const GoldComponent& other) = delete;
		GoldComponent(GoldComponent&& other) = delete;
		GoldComponent& operator=(const GoldComponent& other) = delete;
		GoldComponent& operator=(GoldComponent&& other) = delete;
		void Update(float deltaTime) override;
		void SetState(GoldState* state) {m_pCurrentState = state; }
		bool HasBroken() const;
		bool IsDangerous() const;
	protected:
		bool m_HasFallen{ false };
		bool m_Broken{ false };
		bool m_Dangerous{ false };
		int m_originalCellX{ -1 };
		int m_originalCellY{ -1 };
		LevelManager& m_levelManager{ LevelManager::GetInstance() };
		PlayerAccessor& m_playerAccessor{ dae::serviceLocator::GetPlayerAccessor() };
		GoldState* m_pCurrentState{ nullptr };
		IdleState* m_pIdleState{ nullptr };
		FallingState* m_pFallingState{ nullptr };
		BrokenState* m_pBrokenState{ nullptr };
	private:
		friend class GoldState;
		friend class IdleState;
		friend class FallingState;
		friend class BrokenState;
	};
}
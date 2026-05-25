#pragma once
#include "Component.h"
#include "EnemySpawnManager.h"
#include "LevelManager.h"
#include <glm/vec3.hpp>
#include <TextureComponent.h>

namespace dae
{
	class EnemyState;
	class PlayerComponent;
	class NobbinState;
	class HobbinState;
	class LevelManager;

	class EnemyComponent final : public Component
	{
	public:
		EnemyComponent(GameObject* pOwner, PlayerComponent* pPlayerComponent, TextureComponent* pTextureComponent);
		virtual ~EnemyComponent() = default;
		EnemyComponent(const EnemyComponent& other) = delete;
		EnemyComponent(EnemyComponent&& other) = delete;
		EnemyComponent& operator=(const EnemyComponent& other) = delete;
		EnemyComponent& operator=(EnemyComponent&& other) = delete;
		void Update(float deltaTime) override;
		void SetState(EnemyState* newState);
	protected:
		LevelManager& m_levelManager{ LevelManager::GetInstance() };
		EnemySpawnManager& m_enemySpawnManager{ EnemySpawnManager::GetInstance() };
		TextureComponent* m_pTextureComponent{ nullptr };
		EnemyState* m_pCurrentState{ nullptr };
		NobbinState* m_pNobbinState{ nullptr };
		HobbinState* m_pHobbinState{ nullptr };
		glm::vec3 m_Direction{ 0.f, 0.f, 0.f };
		glm::vec3 m_Position{ 0.f, 0.f, 0.f };
		glm::vec3 m_PlayerPosition{ 0.f, 0.f, 0.f };
	private:
		friend class EnemyState;
		friend class NobbinState;
		friend class HobbinState;
	};
}
#pragma once
#include "Component.h"
#include "LevelManager.h"
#include <glm/vec3.hpp>

namespace dae
{
	class ProjectileComponent final : public Component
	{
	public:
		ProjectileComponent(GameObject* pOwner);
		virtual ~ProjectileComponent() = default;
		ProjectileComponent(const ProjectileComponent& other) = delete;
		ProjectileComponent(ProjectileComponent&& other) = delete;
		ProjectileComponent& operator=(const ProjectileComponent& other) = delete;
		ProjectileComponent& operator=(ProjectileComponent&& other) = delete;
		void Update(float deltaTime) override;
	protected:
		LevelManager* m_pLevelManager{ nullptr };
		TunnelDirection m_Direction{ TunnelDirection::none };
		float m_MoveOffsetDistance{ 0.10f };
		glm::vec3 m_MoveOffset{ 0.f, 0.f, 0.f };
	private:
		glm::vec3 DirectionToMoveOffset(TunnelDirection direction);
	};
}
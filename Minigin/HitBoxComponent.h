#pragma once
#include "Component.h"
#include <glm/vec2.hpp>
#include <vector>

namespace dae
{
	enum class HitboxLayer
	{
		Player,
		Enemy,
		Item
	};

	struct WorldBounds
	{
		float x{};
		float y{};
		float w{};
		float h{};
	};

	class HitBoxComponent final : public Component
	{
	public:
		HitBoxComponent(GameObject* owner, HitboxLayer layer, glm::vec2 size, glm::vec2 offset);
		~HitBoxComponent() override;

		void Update(float deltaTime) override;
		WorldBounds GetWorldBounds() const;
		bool Overlaps(const HitBoxComponent& other) const;
		void Render() const override;

		HitboxLayer GetLayer() const;
		static const std::vector<HitBoxComponent*>& CheckAll();

	private:
		HitboxLayer m_layer{};
		glm::vec2 m_size{};
		glm::vec2 m_offset{};
		bool m_RenderThisHitbox{ false };

		static std::vector<HitBoxComponent*>& GetHitboxes();
	};
}
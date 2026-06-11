#include "HitBoxComponent.h"
#include "GameObject.h"
#include <algorithm>
#include "Renderer.h"

namespace dae
{
	HitBoxComponent::HitBoxComponent(GameObject* owner, HitboxLayer layer, glm::vec2 size, glm::vec2 offset)
		: Component(owner), m_layer(layer), m_size(size), m_offset(offset)
	{
		GetHitboxes().push_back(this);
	}

	HitBoxComponent::~HitBoxComponent()
	{
		GetHitboxes().erase(std::remove(GetHitboxes().begin(), GetHitboxes().end(), this), GetHitboxes().end());
	}

	void HitBoxComponent::Update(float)
	{

	}

	WorldBounds HitBoxComponent::GetWorldBounds() const
	{
		const auto bounds = m_parent->GetWorldPosition();
		return WorldBounds{ bounds.x + m_offset.x, bounds.y + m_offset.y, m_size.x, m_size.y };
	}

	bool HitBoxComponent::Overlaps(const HitBoxComponent& other) const
	{
		const auto bounds = GetWorldBounds();
		const auto otherBounds = other.GetWorldBounds();

		return bounds.x < (otherBounds.x + otherBounds.w) && (bounds.x + bounds.w) > otherBounds.x && bounds.y < (otherBounds.y + otherBounds.h) && (bounds.y + bounds.h) > otherBounds.y;
	}

	void HitBoxComponent::Render() const
	{
		if (m_RenderThisHitbox)
		{
			const auto bounds = GetWorldBounds();
			Renderer::GetInstance().RenderRect(bounds.x, bounds.y, bounds.w, bounds.h, 255, 0, 0);
		}
	}

	HitboxLayer HitBoxComponent::GetLayer() const
	{
		return m_layer;
	}

	const std::vector<HitBoxComponent*>& HitBoxComponent::CheckAll()
	{
		return GetHitboxes();
	}

	std::vector<HitBoxComponent*>& HitBoxComponent::GetHitboxes()
	{
		static auto* registry = new std::vector<HitBoxComponent*>();
		return *registry;
	}
}
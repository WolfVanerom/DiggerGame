#include "HitBoxComponent.h"
#include "GameObject.h"
#include <algorithm>

namespace dae
{
	std::vector<HitBoxComponent*> HitBoxComponent::m_allHitboxes = std::vector<HitBoxComponent*>();

	HitBoxComponent::HitBoxComponent(GameObject* owner, HitboxLayer layer, glm::vec2 size, glm::vec2 offset)
		: Component(owner), m_layer(layer), m_size(size), m_offset(offset)
	{
		m_allHitboxes.push_back(this);
	}

	HitBoxComponent::~HitBoxComponent()
	{
		m_allHitboxes.erase(std::remove(m_allHitboxes.begin(), m_allHitboxes.end(), this), m_allHitboxes.end());
	}

	void HitBoxComponent::Update(float)
	{

	}

	WorldBounds HitBoxComponent::GetWorldBounds() const
	{
		const auto p = m_parent->GetWorldPosition();
		return WorldBounds{ p.x + m_offset.x, p.y + m_offset.y, m_size.x, m_size.y };
	}

	bool HitBoxComponent::Overlaps(const HitBoxComponent& other) const
	{
		const auto a = GetWorldBounds();
		const auto b = other.GetWorldBounds();

		return a.x < (b.x + b.w) && (a.x + a.w) > b.x && a.y < (b.y + b.h) && (a.y + a.h) > b.y;
	}

	HitboxLayer HitBoxComponent::GetLayer() const
	{
		return m_layer;
	}

	const std::vector<HitBoxComponent*>& HitBoxComponent::GetAll()
	{
		return m_allHitboxes;
	}
}
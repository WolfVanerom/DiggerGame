#pragma once
#include <memory>

namespace dae
{
	class GameObject;

	class Component
	{
	protected:
		GameObject* m_parent{};
	public:
		Component(GameObject* parent);
		dae::GameObject* GetParent();
		virtual void Update(float deltaTime);
		virtual void FixedUpdate();
		virtual void Render() const;
		virtual ~Component() = default;
	};
}

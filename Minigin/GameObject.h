#pragma once
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include "Transform.h"
#include "Component.h"
namespace dae
{

	class Texture2D;
	class GameObject final
	{
	protected:
		Transform m_transform{};
		std::vector<std::unique_ptr<Component>> m_components{};
		GameObject* m_parent{};
		std::vector<GameObject*> m_children{};
		bool m_positionDirty{ true };

		void SetLocalPosition(const glm::vec3& pos);
	public:
		void Update(float deltaTime);
		void FixedUpdate();
		void Render() const;
		void MarkForDeletion() { m_markedForDeletion = true; }

		void addComponent(std::unique_ptr<Component> component);
		void removeComponent(Component* component);
		Component* getComponent(const std::type_info& typeInfo) const;
		void hasComponentBeenAdded(Component* component) const;

		void SetParent(GameObject* parent, bool keepWorldPosition);
		GameObject* GetParent() const { return m_parent; }
		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child);
		bool IsChild(const GameObject* potentialChild) const;
		
		const glm::vec3& GetLocalPosition() const;
		const glm::vec3& GetWorldPosition();
		void UpdateWorldPosition();
		void SetPositionDirty();
		void SetPosition(float x, float y, float z = 0);
		void SetPosition(const glm::vec3& pos);

		bool m_markedForDeletion{ false };

		GameObject() = default;
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;
	};
}

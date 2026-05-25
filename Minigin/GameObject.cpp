#include <iostream>
#include <string>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Component.h"

void dae::GameObject::Update(float deltaTime){
	for (const auto& component : m_components)
	{
		component->Update(deltaTime);
	}
}

void dae::GameObject::FixedUpdate(){
	for (const auto& component : m_components)
	{
		component->FixedUpdate();
	}
}

void dae::GameObject::Render() const
{
	for (const auto& component : m_components)
	{
		component->Render();
	}
}

void dae::GameObject::addComponent(std::unique_ptr<dae::Component> component)
{
	m_components.push_back(std::move(component));
}

void dae::GameObject::removeComponent(dae::Component* component)
{
	m_components.erase(std::remove_if(m_components.begin(), m_components.end(), 
		[component](const std::unique_ptr<Component>& ptr) { return ptr.get() == component; }), 
		m_components.end());
}

dae::Component* dae::GameObject::getComponent(const std::type_info& typeInfo) const
{
	for (const auto& component : m_components)
	{
		const Component* ptr = component.get();
		if (typeid(*ptr) == typeInfo)
		{
			return component.get();
		}
	}
	return nullptr;
}

void dae::GameObject::hasComponentBeenAdded(dae::Component* component) const
{
	if (std::find_if(m_components.begin(), m_components.end(),
		[component](const std::unique_ptr<Component>& ptr) { return ptr.get() == component; }) != m_components.end())
	{
		std::cout << "Component has been added to the GameObject." << std::endl;
	}
	else
	{
		std::cout << "Component has not been added to the GameObject." << std::endl;
	}
}

void dae::GameObject::SetParent(GameObject* parent, bool keepWorldPosition)
{
	if (IsChild(parent) or parent == this or m_parent == parent) {
		return;
	}
	if (parent == nullptr) {
		SetLocalPosition(GetWorldPosition());
	}
	else
	{
		if (keepWorldPosition) {
			SetLocalPosition(GetWorldPosition() - parent->GetWorldPosition());
		}
		SetPositionDirty();
	}
	if (m_parent) {
		m_parent->RemoveChild(this);
	}
	m_parent = parent;
	if (m_parent) {
		m_parent->AddChild(this);
	}
}

void dae::GameObject::AddChild(GameObject* child)
{
	if (child == nullptr or IsChild(child) or child == this) {
		return;
	}
	child->SetParent(this, true);
	m_children.push_back(child);
}

void dae::GameObject::RemoveChild(GameObject* child)
{
	m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());
}

void dae::GameObject::SetPosition(float x, float y, float z)
{
	SetLocalPosition(glm::vec3(x, y, z));
}

void dae::GameObject::SetPosition(const glm::vec3& pos)
{
	SetLocalPosition(pos);
}

dae::GameObject::~GameObject()
{
	for (auto& child : m_children)
	{
		child->SetParent(nullptr, false);
	}
	m_children.clear();
	m_components.clear();
	if (m_parent) {
		m_parent->RemoveChild(this);
	}
	m_parent = nullptr;
	m_positionDirty = true;
	m_transform.SetWorldPosition(glm::vec3(0, 0, 0));
	m_transform.SetLocalPosition(glm::vec3(0, 0, 0));
}

void dae::GameObject::SetLocalPosition(const glm::vec3& pos)
{
	m_transform.SetLocalPosition(pos);
	SetPositionDirty();
}

const glm::vec3& dae::GameObject::GetWorldPosition()
{
	if (m_positionDirty) {
		UpdateWorldPosition();
	}
	return m_transform.GetWorldPosition();
}

void dae::GameObject::UpdateWorldPosition()
{
	if (m_positionDirty)
	{
		if (m_parent == nullptr) {
			m_transform.SetWorldPosition(m_transform.GetLocalPosition());
		}
		else {
			m_transform.SetWorldPosition(m_parent->GetWorldPosition() + m_transform.GetLocalPosition());
		}
	}
	m_positionDirty = false;
}

void dae::GameObject::SetPositionDirty()
{
	m_positionDirty = true;
	for (auto& child : m_children)
	{
		child->SetPositionDirty();
	}
}

bool dae::GameObject::IsChild(const GameObject* potentialChild) const
{
	for (const auto& child : m_children)
	{
		if (child == potentialChild or child->IsChild(potentialChild))
		{
			return true;
		}
	}
	return false;
}
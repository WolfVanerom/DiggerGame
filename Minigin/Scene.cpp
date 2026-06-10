#include <algorithm>
#include "Scene.h"

using namespace dae;

Scene::Scene(const std::function<void()>& onSceneEnter, const std::function<void()>& onSceneExit)
	: m_onSceneEnter(onSceneEnter), m_onSceneExit(onSceneExit)
{
}

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_objects.emplace_back(std::move(object));
}

void dae::Scene::InsertAt(size_t index, std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot insert a null GameObject into the scene.");
	if (index > m_objects.size())
	{
		index = m_objects.size();
	}
	m_objects.insert(m_objects.begin() + static_cast<int>(index), std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[&object](const auto& ptr) { return ptr.get() == &object; }
		),
		m_objects.end()
	);
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void Scene::Update(float deltaTime)
{
	for(auto& object : m_objects)
	{
		object->Update(deltaTime);
	}
}

void Scene::FixedUpdate()
{
	for (auto& object : m_objects)
	{
		object->FixedUpdate();
	}
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->Render();
	}
}

void dae::Scene::CheckForDeletion()
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[](const auto& ptr) { return ptr->m_markedForDeletion; }
		),
		m_objects.end()
	);
}

void Scene::OnSceneEnter() const
{
	if (m_onSceneEnter != nullptr) {
		m_onSceneEnter();
	}
}

void Scene::OnSceneExit() const
{
	if (m_onSceneExit != nullptr) {
		m_onSceneExit();
	}
}


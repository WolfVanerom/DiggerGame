#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update(float deltaTime)
{
	for(auto& scene : m_scenes)
	{
		scene->Update(deltaTime);
	}
}

void dae::SceneManager::UpdateOneScene(float deltaTime)
{
	if (m_pActiveScene)
	{
		m_pActiveScene->Update(deltaTime);
	}
}

void dae::SceneManager::FixedUpdate()
{
	for (auto& scene : m_scenes)
	{
		scene->FixedUpdate();
	}
}

void dae::SceneManager::FixedUpdateOneScene()
{
	if (m_pActiveScene)
	{
		m_pActiveScene->FixedUpdate();
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_scenes)
	{
		scene->Render();
	}
}

void dae::SceneManager::RenderOneScene()
{
	if (m_pActiveScene)
	{
		m_pActiveScene->Render();
	}
}

void dae::SceneManager::SetActiveScene(Scene* pScene)
{
	m_pActiveScene = pScene;
}

void dae::SceneManager::CheckForDeletion()
{
	for (const auto& scene : m_scenes)
	{
		scene->CheckForDeletion();
	}
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_scenes.emplace_back(std::unique_ptr<Scene>(new Scene()));
	return *m_scenes.back();
}
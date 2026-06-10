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
	if (m_pActiveScene != nullptr)
	{
		m_pActiveScene->OnSceneExit();
	}
	m_pActiveScene = pScene;
	m_pActiveScene->OnSceneEnter();
}

void dae::SceneManager::OnSceneEnter(Scene* pScene)
{
	if (pScene)
	{
		pScene->OnSceneEnter();
	}
}

void dae::SceneManager::OnSceneExit(Scene* pScene)
{
	if (pScene)
	{
		pScene->OnSceneExit();
	}
}

void dae::SceneManager::CheckForDeletion()
{
	for (const auto& scene : m_scenes)
	{
		scene->CheckForDeletion();
	}
}

dae::Scene& dae::SceneManager::CreateScene(const std::function<void()>& onSceneEnter, const std::function<void()>& onSceneExit)
{
	m_scenes.emplace_back(std::unique_ptr<Scene>(new Scene(onSceneEnter, onSceneExit)));
	return *m_scenes.back();
}

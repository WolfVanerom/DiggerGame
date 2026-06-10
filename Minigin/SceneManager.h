#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "Scene.h"
#include "Singleton.h"

namespace dae
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene(const std::function<void()>& onSceneEnter = []() {}, const std::function<void()>& onSceneExit = []() {});

		void Update(float deltaTime);
		void UpdateOneScene(float deltaTime);
		void FixedUpdate();
		void FixedUpdateOneScene();
		void Render();
		void RenderOneScene();

		void SetActiveScene(Scene* pScene);
		void OnSceneEnter(Scene* pScene);
		void OnSceneExit(Scene* pScene);

		void CheckForDeletion();
	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::unique_ptr<Scene>> m_scenes{};
		Scene* m_pActiveScene{ nullptr };
	};
}

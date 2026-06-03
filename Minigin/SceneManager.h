#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Scene.h"
#include "Singleton.h"

namespace dae
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene();

		void Update(float deltaTime);
		void UpdateOneScene(float deltaTime);
		void FixedUpdate();
		void FixedUpdateOneScene();
		void Render();
		void RenderOneScene();

		void SetActiveScene(Scene* pScene);

		void CheckForDeletion();
	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::unique_ptr<Scene>> m_scenes{};
		Scene* m_pActiveScene{ nullptr };
	};
}

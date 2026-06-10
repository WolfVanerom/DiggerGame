#pragma once
#include <memory>
#include <string>
#include <vector>
#include "GameObject.h"

namespace dae
{
	class Scene final
	{
	public:
		void Add(std::unique_ptr<GameObject> object);
		void InsertAt(size_t index, std::unique_ptr<GameObject> object);
		void Remove(const GameObject& object);
		void RemoveAll();

		void Update(float deltaTime);
		void FixedUpdate();
		void Render() const;
		void CheckForDeletion();

		void OnSceneEnter() const;
		void OnSceneExit() const;

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		friend class SceneManager;
		explicit Scene(const std::function<void()>& onSceneEnter = []() {}, const std::function<void()>& onSceneExit = []() {});

		std::vector < std::unique_ptr<GameObject>> m_objects{};
		const std::function<void()> m_onSceneEnter{nullptr};
		const std::function<void()> m_onSceneExit{nullptr};
	};

}

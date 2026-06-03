#pragma once
#include "Component.h"
#include <vector>

namespace dae
{
	class ButtonComponent;
	class Scene;
	class MenuComponent final : public Component
	{
	public:
		MenuComponent(GameObject* pOwner, Scene* pScene);
		~MenuComponent() override = default;
		MenuComponent(const MenuComponent& other) = delete;
		MenuComponent(MenuComponent&& other) noexcept = delete;
		MenuComponent& operator=(const MenuComponent& other) = delete;
		MenuComponent& operator=(MenuComponent&& other) noexcept = delete;
		void MoveRight();
		void MoveLeft();
		void MoveUp();
		void MoveDown();
		void Update(float deltaTime) override;
		void AddMenuItem(ButtonComponent* pButton, unsigned int column);
		void ClickSelectedItem();
		void SetSelectedItem(ButtonComponent* pButton);
	private:
		std::vector<std::vector<ButtonComponent*>> m_pMenuItems{};
		ButtonComponent* m_pSelectedItem{};
		void HighlightSelectedItem();
		Scene* m_pScene{};
	};
}
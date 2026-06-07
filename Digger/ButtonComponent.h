#pragma once
#include "Component.h"
#include <functional>

namespace dae
{
	class TextComponent;
	class ButtonComponent final : public Component
	{
	public:
		ButtonComponent(GameObject* pOwner, TextComponent* pTextComponent, const std::function<void()>& onClick);
		~ButtonComponent() override = default;
		ButtonComponent(const ButtonComponent& other) = delete;
		ButtonComponent(ButtonComponent&& other) noexcept = delete;
		ButtonComponent& operator=(const ButtonComponent& other) = delete;
		ButtonComponent& operator=(ButtonComponent&& other) noexcept = delete;
		void Update(float deltaTime) override;
		void OnClick();
		void Highlight();
		void Unhighlight();
	private:
		TextComponent* m_pTextComponent{};
		const std::function<void()> m_onClick;
	};
}
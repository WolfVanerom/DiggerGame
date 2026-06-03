#include "ButtonComponent.h"
#include "TextComponent.h"

namespace dae {
	ButtonComponent::ButtonComponent(GameObject* pOwner, TextComponent* pTextComponent, const std::function<void()>& onClick)
		: Component(pOwner), m_pTextComponent(pTextComponent), m_onClick(onClick)
	{
	}

	void ButtonComponent::Update(float)
	{
	}

	void ButtonComponent::OnClick()
	{
		m_onClick();
	}

	void ButtonComponent::Highlight()
	{
		m_pTextComponent->SetColor({ 255, 0, 0, 255 });
	}

	void ButtonComponent::Unhighlight()
	{
		m_pTextComponent->SetColor({ 255, 255, 255, 255 });
	}
}
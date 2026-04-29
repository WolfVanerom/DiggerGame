#include <FPSComponent.h>
#include <TextComponent.h>
#include "GameObject.h"

dae::FPSComponent::FPSComponent(GameObject* pOwner)
	: Component(pOwner), m_pTextComponent(nullptr), m_accumulatedTime(0.0f), m_frameCount(0)
{
}

void dae::FPSComponent::Update(float deltaTime)
{
	if (!m_pTextComponent)
	{
		m_pTextComponent = dynamic_cast<TextComponent*>(m_parent->getComponent(typeid(TextComponent)));
		if (!m_pTextComponent)
		{
			return;
		}
	}

	m_accumulatedTime += deltaTime;
	m_frameCount++;
	if (m_accumulatedTime >= 0.1f)
	{
		lastFps = fps;
		fps = m_frameCount / m_accumulatedTime;
		if (lastFps != fps) {
			m_pTextComponent->SetText("FPS: " + std::to_string(static_cast<int>(fps)));
		}
		m_accumulatedTime = 0.0f;
		m_frameCount = 0;
	}
}
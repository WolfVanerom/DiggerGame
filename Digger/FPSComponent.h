#pragma once
#include "Component.h"

namespace dae
{
	class TextComponent;
	class FPSComponent final : public Component
	{
	public:
		void Update(float deltaTime) override;
		explicit FPSComponent(GameObject* pOwner);
		virtual ~FPSComponent() = default;
		FPSComponent(const FPSComponent& other) = delete;
		FPSComponent(FPSComponent&& other) = delete;
		FPSComponent& operator=(const FPSComponent& other) = delete;
		FPSComponent& operator=(FPSComponent&& other) = delete;
	private:
		TextComponent* m_pTextComponent{};
		float m_accumulatedTime{};
		int m_frameCount{};
		float fps{};
		float lastFps{};
	};
}

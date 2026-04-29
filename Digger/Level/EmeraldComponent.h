#pragma once
#include "Component.h"

namespace dae {
	class GameObject;
	class EmeraldComponent final : public Component
	{
	public:
		EmeraldComponent(GameObject* pOwner);
		virtual ~EmeraldComponent() = default;
		EmeraldComponent(const EmeraldComponent& other) = delete;
		EmeraldComponent(EmeraldComponent&& other) = delete;
		EmeraldComponent& operator=(const EmeraldComponent& other) = delete;
		EmeraldComponent& operator=(EmeraldComponent&& other) = delete;
		void giveEmerald() const;
	};

}
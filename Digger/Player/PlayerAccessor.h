#pragma once
#include <Singleton.h>

namespace dae
{
	class PlayerComponent;
	class PlayerAccessor final : public Singleton<PlayerAccessor>
	{
	public:
		PlayerComponent* GetPlayer() const;
		void SetPlayer(PlayerComponent* pPlayer);
	private:
		PlayerComponent* m_pPlayer{ nullptr };
	};
}
#pragma once

namespace dae {
	class GamePadSystem
	{
	public:
		virtual ~GamePadSystem() = default;
		virtual void Initialize() = 0;
		virtual int GetPlayerIdFromIndex(int index) const = 0;
	};
}
#pragma once

namespace dae {
	class GamePadSystem
	{
	public:
		virtual ~GamePadSystem() = default;
		virtual void Initialize() = 0;
		virtual int GetPlayerIdFromIndex(size_t index) const = 0;
	};
}
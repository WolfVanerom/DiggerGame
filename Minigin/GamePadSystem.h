#pragma once

namespace dae {
	class GamePadSystem
	{
	public:
		virtual ~GamePadSystem() = default;
		virtual void update() = 0;
		virtual void Initialize() = 0;
	};
}
#pragma once
#include <string>

using soundId = unsigned int;
namespace dae{
	class soundSystem
	{
	public:
		virtual ~soundSystem() = default;
		virtual void playSound(const soundId id, const float volume) = 0;
		virtual void registerSound(const soundId id, const std::string& path) = 0;
	};
}
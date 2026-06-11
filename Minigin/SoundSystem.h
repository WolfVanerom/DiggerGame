#pragma once
#include <string>

using soundId = unsigned int;
namespace dae{
	class soundSystem
	{
	public:
		virtual ~soundSystem() = default;
		virtual void playSound(const soundId id, const float volume, const bool loop) = 0;
		virtual void registerSound(const soundId id, const std::string& path) = 0;
		virtual void pauseSound(const soundId id) = 0;
		virtual void resumeSound(const soundId id) = 0;
		virtual void pauseAllSounds() = 0;
		virtual void resumeAllSounds() = 0;
		virtual bool getIsMuted() const = 0;
	};
}
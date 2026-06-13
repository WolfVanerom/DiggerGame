#pragma once
#include "SoundSystem.h"

namespace dae
{
	class NullSoundSystem final : public soundSystem
	{
	public:
		NullSoundSystem() = default;
		~NullSoundSystem() override = default;
		void playSound(const soundId id, const float volume, const bool loop) override {}
		void registerSound(const soundId id, const std::string& path) override {}
		void pauseSound(const soundId id) override {}
		void resumeSound(const soundId id) override {}
		void pauseAllSounds(bool mute) override {}
		void resumeAllSounds() override {}
		bool getIsMuted() const override { return false; }
	};
}
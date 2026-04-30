#pragma once
#include "SoundSystem.h"
#ifdef SDL3_mixer_FOUND
#include <SDL3_mixer/SDL_mixer.h>
#endif
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace dae
{
#ifdef SDL3_mixer_FOUND
	class SdlSoundSystem final : public soundSystem
	{
	public:
		SdlSoundSystem();
		~SdlSoundSystem() override;

		void playSound(const soundId id, const float volume) override;
		void registerSound(const soundId id, const std::string& path) override;

	protected:
		void processSound(const soundId id, const float volume);

	private:
		struct SoundEvent
		{
			soundId id{};
			float volume{};
		};

		void workerLoop();
		MIX_Audio* getOrLoadAudio(const soundId id);
		void cleanupFinishedTracks();

		std::mutex m_queueMutex{};
		std::condition_variable m_queueCv{};
		std::queue<SoundEvent> m_pendingEvents{};
		bool m_running{ true };
		std::thread m_workerThread{};

		MIX_Mixer* m_mixer{};
		std::unordered_map<soundId, std::string> m_soundPaths{};
		std::unordered_map<soundId, MIX_Audio*> m_loadedAudio{};
		std::vector<MIX_Track*> m_activeTracks{};
	};
#else
	// Stub implementation when SDL3_mixer is not available
	class SdlSoundSystem final : public soundSystem
	{
	public:
		SdlSoundSystem() = default;
		~SdlSoundSystem() override = default;

		void playSound(const soundId, const float) override {}
		void registerSound(const soundId, const std::string&) override {}
	};
#endif
}
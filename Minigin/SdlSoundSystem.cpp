#include "SdlSoundSystem.h"
#include <SDL3/SDL.h>
#include <algorithm>

namespace dae
{
	SdlSoundSystem::SdlSoundSystem()
	{
		if (!MIX_Init())
		{
			return;
		}

		m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
		if (m_mixer == nullptr)
		{
			return;
		}

		m_workerThread = std::thread(&SdlSoundSystem::workerLoop, this);
	}

	SdlSoundSystem::~SdlSoundSystem()
	{
		{
			std::lock_guard lock(m_queueMutex);
			m_running = false;
		}
		m_queueCv.notify_one();
		if (m_workerThread.joinable())
		{
			m_workerThread.join();
		}

		for (auto track : m_activeTracks)
		{
			if (track != nullptr)
			{
				MIX_DestroyTrack(track);
			}
		}
		m_activeTracks.clear();

		if (m_mixer != nullptr)
		{
			MIX_DestroyMixer(m_mixer);
			m_mixer = nullptr;
		}

		MIX_Quit();
	}

	void SdlSoundSystem::playSound(const soundId id, const float volume, const bool loop)
	{
		if (m_isMuted) {
			return;
		}
		std::lock_guard lock(m_queueMutex);
		m_pendingEvents.push(SoundEvent{ id, std::clamp(volume, 0.0f, 1.0f), loop });
		m_queueCv.notify_one();
	}

	void SdlSoundSystem::registerSound(const soundId id, const std::string& path)
	{
		std::lock_guard lock(m_audioMutex);
		m_soundPaths[id] = path;
	}

	void SdlSoundSystem::pauseSound(const soundId id)
	{
		std::lock_guard lock(m_audioMutex);
		for (auto track : m_activeTracks)
		{
			if (track != nullptr && MIX_GetTrackAudio(track) == getOrLoadAudio(id))
			{
				MIX_PauseTrack(track);
			}
		}
	}

	void SdlSoundSystem::resumeSound(const soundId id)
	{
		std::lock_guard lock(m_audioMutex);
		for (auto track : m_activeTracks)
		{
			if (track != nullptr && MIX_GetTrackAudio(track) == getOrLoadAudio(id))
			{
				MIX_ResumeTrack(track);
			}
		}
	}

	void SdlSoundSystem::pauseAllSounds(bool mute)
	{
		m_isMuted = mute;
		std::lock_guard lock(m_audioMutex);
		for (auto track : m_activeTracks)
		{
			if (track != nullptr)
			{
				MIX_PauseTrack(track);
			}
		}
	}

	void SdlSoundSystem::resumeAllSounds()
	{
		m_isMuted = false;
		std::lock_guard lock(m_audioMutex);
		for (auto track : m_activeTracks)
		{
			if (track != nullptr)
			{
				MIX_ResumeTrack(track);
			}
		}
	}

	bool SdlSoundSystem::getIsMuted() const
	{
		return m_isMuted;
	}

	void SdlSoundSystem::processSound(const soundId id, const float volume, const bool loop)
	{
		if (m_mixer == nullptr)
		{
			return;
		}

		MIX_Audio* audio;
		{
			std::lock_guard lock(m_audioMutex);
			audio = getOrLoadAudio(id);

			if (audio == nullptr)
			{
				return;
			}

			SDL_PropertiesID props = SDL_CreateProperties();

			SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loop ? -1 : 0);

			for (auto track : m_activeTracks)
			{
				if (track != nullptr && MIX_GetTrackAudio(track) == audio)
				{
					MIX_PlayTrack(track, props);
				}
			}

			MIX_Track* track = MIX_CreateTrack(m_mixer);
			if (track == nullptr)
			{
				return;
			}

			if (!MIX_SetTrackAudio(track, audio))
			{
				MIX_DestroyTrack(track);
				return;
			}

			if (!MIX_SetTrackGain(track, std::clamp(volume, 0.0f, 1.0f)))
			{
			}

			if (!MIX_PlayTrack(track, props))
			{
				MIX_DestroyTrack(track);
				return;
			}

			m_activeTracks.push_back(track);
		}
	}

	void SdlSoundSystem::workerLoop()
	{
		while (true)
		{
			SoundEvent event{};
			{
				std::unique_lock lock(m_queueMutex);
				m_queueCv.wait(lock, [this]
				{
					return !m_running or !m_pendingEvents.empty();
				});

				if (!m_running && m_pendingEvents.empty())
				{
					break;
				}

				event = m_pendingEvents.front();
				m_pendingEvents.pop();
			}

			processSound(event.id, event.volume, event.loop);
			cleanupFinishedTracks();
		}
	}

	MIX_Audio* SdlSoundSystem::getOrLoadAudio(const soundId id)
	{
		if (const auto loadedIt = m_loadedAudio.find(id); loadedIt != m_loadedAudio.end())
		{
			return loadedIt->second;
		}

		const auto pathIt = m_soundPaths.find(id);
		if (pathIt == m_soundPaths.end())
		{
			return nullptr;
		}

		MIX_Audio* audio = MIX_LoadAudio(m_mixer, pathIt->second.c_str(), true);
		if (audio == nullptr)
		{
			return nullptr;
		}

		m_loadedAudio[id] = audio;
		return audio;
	}

	void SdlSoundSystem::cleanupFinishedTracks()
	{
		std::lock_guard lock(m_audioMutex);
		if (m_isMuted) {
			return;
		}
		auto newEnd = std::remove_if(m_activeTracks.begin(), m_activeTracks.end(), [](MIX_Track* track)
		{
				if (track == nullptr)
				{
					return true;
				}

				if (!MIX_TrackPlaying(track))
				{
					MIX_DestroyTrack(track);
					return true;
				}

				return false;
		});

		m_activeTracks.erase(newEnd, m_activeTracks.end());
	}
}
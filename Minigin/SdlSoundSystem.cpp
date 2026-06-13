#include "SdlSoundSystem.h"
#include <SDL3/SDL.h>
#include <algorithm>

namespace dae
{
	SdlSoundSystem::SdlSoundSystem()
	{
		m_running = true;
		m_isMuted = false;

		if (!MIX_Init())
		{
			m_enabled = false;
			return;
		}

		m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
		if (m_mixer == nullptr)
		{
			m_enabled = false;
			MIX_Quit();
			return;
		}

		m_enabled = true;
		m_initialized = true;

		m_workerThread = std::thread(&SdlSoundSystem::workerLoop, this);
	}

	SdlSoundSystem::~SdlSoundSystem()
	{
		m_running = false;
		m_queueCv.notify_one();

		if (m_workerThread.joinable())
		{
			m_workerThread.join();
		}

		if (!m_initialized) {
			return;
		}

		for (auto track : m_activeTracks)
		{
			if (track)
				MIX_DestroyTrack(track);
		}
		m_activeTracks.clear();

		if (m_mixer)
		{
			MIX_DestroyMixer(m_mixer);
			m_mixer = nullptr;
		}

		MIX_Quit();
	}

	void SdlSoundSystem::playSound(const soundId id, const float volume, const bool loop)
	{
		if (!m_enabled || m_isMuted) {
			return;
		}

		std::lock_guard lock(m_queueMutex);
		m_pendingEvents.push(SoundEvent{ id, std::clamp(volume, 0.0f, 1.0f), loop });
		m_queueCv.notify_one();
	}

	void SdlSoundSystem::registerSound(const soundId id, const std::string& path)
	{
		if (!m_enabled) {
			return;
		}

		std::lock_guard lock(m_audioMutex);
		m_soundPaths[id] = path;
	}

	void SdlSoundSystem::pauseSound(const soundId id)
	{
		if (!m_enabled) {
			return;
		}

		std::lock_guard lock(m_audioMutex);

		for (auto track : m_activeTracks)
		{
			if (track && MIX_GetTrackAudio(track) == getOrLoadAudio(id))
				MIX_PauseTrack(track);
		}
	}

	void SdlSoundSystem::resumeSound(const soundId id)
	{
		if (!m_enabled) {
			return;
		}
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
		if (!m_enabled) {
			return;
		}

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
		if (!m_enabled) {
			return;
		}

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
		if (!m_enabled || m_mixer == nullptr) {
			return;
		}

		MIX_Audio* audio = nullptr;

		{
			std::lock_guard lock(m_audioMutex);
			audio = getOrLoadAudio(id);

			if (!audio)
				return;
		}

		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loop ? -1 : 0);

		MIX_Track* track = MIX_CreateTrack(m_mixer);
		if (!track)
			return;

		if (!MIX_SetTrackAudio(track, audio))
		{
			MIX_DestroyTrack(track);
			return;
		}

		MIX_SetTrackGain(track, std::clamp(volume, 0.0f, 1.0f));

		if (!MIX_PlayTrack(track, props))
		{
			MIX_DestroyTrack(track);
			return;
		}

		std::lock_guard lock(m_audioMutex);
		m_activeTracks.push_back(track);
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
						return !m_running || !m_pendingEvents.empty();
					});

				if (!m_running && m_pendingEvents.empty())
					break;

				event = m_pendingEvents.front();
				m_pendingEvents.pop();
			}

			processSound(event.id, event.volume, event.loop);

			if (m_enabled) {
				cleanupFinishedTracks();
			}
		}
	}

	MIX_Audio* SdlSoundSystem::getOrLoadAudio(const soundId id)
	{
		if (!m_enabled) {
			return nullptr;
		}

		if (auto it = m_loadedAudio.find(id); it != m_loadedAudio.end())
			return it->second;

		auto pathIt = m_soundPaths.find(id);
		if (pathIt == m_soundPaths.end())
			return nullptr;

		MIX_Audio* audio = MIX_LoadAudio(m_mixer, pathIt->second.c_str(), true);
		if (!audio)
			return nullptr;

		m_loadedAudio[id] = audio;
		return audio;
	}

	void SdlSoundSystem::cleanupFinishedTracks()
	{
		if (!m_enabled) {
			return;
		}

		std::lock_guard lock(m_audioMutex);

		auto newEnd = std::remove_if(m_activeTracks.begin(), m_activeTracks.end(),
			[](MIX_Track* track)
			{
				if (!track)
					return true;

				if (!MIX_TrackPlaying(track))
				{
					MIX_DestroyTrack(track);
					return true;
				}

				return false;
			});
		m_activeTracks.erase(newEnd, m_activeTracks.end());
	}
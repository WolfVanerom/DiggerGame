#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <string>

namespace dae
{
	class Minigin final
	{
		bool m_quit{};
		std::chrono::high_resolution_clock::time_point m_lastTime = std::chrono::high_resolution_clock::now();
		float m_accLag{0.0f};
		int m_sleepTime{16};
		float m_fixedTimeStep{ 0.016f };
		float m_currentFPS{ 0.0f };
		int m_windowWidth{ 1024 };
		int m_windowHeight{ 576 };
		std::string m_WindowTitle{ "Digger" };
	public:
		explicit Minigin(const std::filesystem::path& dataPath);
		~Minigin();
		void Run(const std::function<void()>& load);
		void RunOneFrame();
		float GetFPS();

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;
	};
}
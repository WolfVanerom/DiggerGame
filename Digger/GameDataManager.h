#pragma once
#include <memory>
#include <vector>
#include <string>
#include <Scene.h>
namespace dae
{
	enum class GameMode
	{
		SinglePlayer,
		Coop,
		Versus
	};

	class GameDataManager final
	{
	public:
		GameDataManager() = default;
		GameDataManager(const GameDataManager& other) = delete;
		GameDataManager(GameDataManager&& other) noexcept = delete;
		GameDataManager& operator=(const GameDataManager& other) = delete;
		GameDataManager& operator=(GameDataManager&& other) noexcept = delete;
		~GameDataManager() = default;

		std::string GetHighScorePath() const;
		void AddCurrentScore(int score);
		int GetCurrentScore() const;

		void SetGameMode(GameMode mode);
		GameMode GetGameMode() const;

		void RegisterScoreSaveScene(Scene* scene);
		Scene* GetScoreSaveScene() const;

		void SetWindowSize(const glm::vec2& size);
		glm::vec2 GetWindowSize() const;

		void SetWindowTitle(const std::string& title);
		std::string GetWindowTitle() const;
	private:
		const std::string m_HighScoreFilePath{ "highscores.txt" };
		int m_CurrentScore{ 0 };
		GameMode m_GameMode{ GameMode::SinglePlayer };
		Scene* m_ScoreSaveScene{ nullptr };
		glm::vec2 m_WindowSize{ 1024, 576 };
		std::string m_WindowTitle{ "Digger" };
	};
}
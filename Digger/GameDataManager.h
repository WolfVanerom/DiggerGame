#pragma once
#include <memory>
#include <vector>
#include <string>
namespace dae
{
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
		void SetCurrentScore(int score);
		int GetCurrentScore() const;
	private:
		const std::string m_HighScoreFilePath{ "highscores.txt" };
		int m_CurrentScore{ 0 };
	};
}
#include "GameDataManager.h"

namespace dae
{
	std::string GameDataManager::GetHighScorePath() const
	{
		return m_HighScoreFilePath;
	}

	void GameDataManager::SetCurrentScore(int score)
	{
		m_CurrentScore = score;
	}

	int GameDataManager::GetCurrentScore() const
	{
		return m_CurrentScore;
	}
}
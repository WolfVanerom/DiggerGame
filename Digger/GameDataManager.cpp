#include "GameDataManager.h"

namespace dae
{
	std::string GameDataManager::GetHighScorePath() const
	{
		return m_HighScoreFilePath;
	}

	void GameDataManager::AddCurrentScore(int score)
	{
		m_CurrentScore += score;
	}

	int GameDataManager::GetCurrentScore() const
	{
		return m_CurrentScore;
	}

	void GameDataManager::SetGameMode(GameMode mode)
	{
		m_GameMode = mode;
	}

	GameMode GameDataManager::GetGameMode() const
	{
		return m_GameMode;
	}

	void GameDataManager::RegisterScoreSaveScene(Scene* scene)
	{
		m_ScoreSaveScene = scene;
	}

	Scene* GameDataManager::GetScoreSaveScene() const
	{
		return m_ScoreSaveScene;
	}
	void GameDataManager::SetWindowSize(const glm::vec2& size)
	{
		m_WindowSize = size;
	}

	glm::vec2 GameDataManager::GetWindowSize() const
	{
		return m_WindowSize;
	}

	void GameDataManager::SetWindowTitle(const std::string& title)
	{
		m_WindowTitle = title;
	}

	std::string GameDataManager::GetWindowTitle() const
	{
		return m_WindowTitle;
	}
}
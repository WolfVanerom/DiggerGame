#pragma once
#include "Singleton.h"
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <map>

namespace dae
{
	enum class LevelObjectType
	{
		empty,
		tunnelEnd,
		horizontalTunnel,
		verticalTunnel,
		bag,
		gold,
		emerald,
		none
	};

	enum class TunnelDirection
	{
		none,
		left,
		right,
		up,
		down
	};

	struct TunnelPreview
	{
		bool active{ false };
		int cellX{ -1 };
		int cellY{ -1 };
		LevelObjectType type{ LevelObjectType::none };
		TunnelDirection direction{ TunnelDirection::none };
		float progress{ 0.f };
	};


	class Scene;
	class GameObject;
	class LevelManager final : public Singleton<LevelManager>
	{
	private:
		GameObject* m_currentNonEntetyDraw{ nullptr };
		GameObject* m_currentBackgroundObject{ nullptr };
		std::vector<std::vector<GameObject*>> m_EntityObjects{};
		std::vector<std::vector<LevelObjectType>> m_tileObjects{};
		std::map<GameObject*, TunnelPreview> m_tunnelPreviews{};

		std::vector<std::string> m_currentLevel;
		Scene* m_currentScene{ nullptr };
		std::optional<std::pair<std::string, Scene*>> m_pendingLevelLoad;

		void CreateCurrentNonEntityDrawObject(Scene* scene);
		void CreateCurrentBackgroundObject(Scene* scene);
		void SpawnLevelObject(LevelObjectType type, int x, int y, Scene* scene);

		LevelObjectType CharToType(char c) const;
		char TypeToChar(LevelObjectType type) const;
		std::string_view GetTextureForType(LevelObjectType type) const;
		bool IsInBounds(int x, int y) const;
	protected:
		static constexpr int m_maxWidth{ 15 };
		static constexpr int m_maxHeight{ 10 };
		static constexpr float m_windowWidth{ 1024.f };
		static constexpr float m_windowHeight{ 576.f };
		int m_amountOfEmeralds{ -1 };
		int m_currentLevelIndex{ 1 };
	public:
		static constexpr float m_tileWidth = m_windowWidth / static_cast<float>(m_maxWidth);
		static constexpr float m_tileHeight = m_windowHeight / static_cast<float>(m_maxHeight);

		void LoadLevel(const std::string& levelFile, Scene* scene);
		void ClearLevel();
		void CheckIfLevelCompleted();
		void ProcessPendingLevelLoad();
		void QueueLevelLoad(const std::string& levelFile, Scene* scene);
		void LowerEmeraldCount() { m_amountOfEmeralds--;}

		LevelObjectType GetCell(int x, int y) const;
		void SetCell(int x, int y, LevelObjectType type);
		void MoveEntityCell(int fromX, int fromY, int toX, int toY, LevelObjectType newType, bool moveTexture);
		void SetTunnelPreview(GameObject* player, int cellX, int cellY, LevelObjectType type, TunnelDirection direction, float progress);
		void ClearTunnelPreview(GameObject* player);
		std::vector<TunnelPreview> GetAllTunnelPreviews() const;
	};
}
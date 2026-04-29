#pragma once

#include "Component.h"
#include "LevelManager.h"
#include <map>
#include <memory>
#include <vector>

namespace dae
{
	class Texture2D;

	class TileMapComponent final : public Component
	{
	private:
       const std::vector<std::vector<LevelObjectType>>* m_tiles{};
		std::map<LevelObjectType, std::shared_ptr<Texture2D>> m_tileTextures{};
		float m_tileWidth;
		float m_tileHeight;

		void RenderTiledTexture(const Texture2D& texture, float x, float y, float width, float height, bool tileHorizontally) const;
		LevelObjectType GetConnectedTunnelType(size_t x, size_t y) const;
     void RenderTunnelPreview() const;
	public:
		explicit TileMapComponent(GameObject* pOwner, float tileWidth = 16.f, float tileHeight = 16.f);
		~TileMapComponent() override = default;
		TileMapComponent(const TileMapComponent& other) = delete;
		TileMapComponent(TileMapComponent&& other) noexcept = delete;
		TileMapComponent& operator=(const TileMapComponent& other) = delete;
		TileMapComponent& operator=(TileMapComponent&& other) noexcept = delete;

		void Render() const override;

		void SetTiles(const std::vector<std::vector<LevelObjectType>>* tiles);
		void SetTileTexture(LevelObjectType type, const std::string& filename);
		void SetTileSize(float tileWidth, float tileHeight);
	};
}

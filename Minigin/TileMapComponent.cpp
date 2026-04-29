#include "TileMapComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include <algorithm>
#include <iostream>

namespace dae
{
	static bool IsHorizontalConnector(const LevelObjectType type)
	{
		return type == LevelObjectType::horizontalTunnel || type == LevelObjectType::tunnelEnd;
	}

	static bool IsVerticalConnector(const LevelObjectType type)
	{
		return type == LevelObjectType::verticalTunnel || type == LevelObjectType::tunnelEnd;
	}

	TileMapComponent::TileMapComponent(GameObject* pOwner, float tileWidth, float tileHeight)
		: Component(pOwner)
		, m_tileWidth(tileWidth)
		, m_tileHeight(tileHeight)
	{
	}

	void TileMapComponent::RenderTiledTexture(const Texture2D& texture, float x, float y, float width, float height, bool tileHorizontally) const
	{
		const glm::vec2 textureSize = texture.GetSize();
		if (textureSize.x <= 0.f || textureSize.y <= 0.f)
		{
			return;
		}

		if (tileHorizontally)
		{
			for (float xOffset = 0.f; xOffset < width; xOffset += textureSize.x)
			{
				const float drawWidth = std::min(textureSize.x, width - xOffset);
				Renderer::GetInstance().RenderTexture(texture, x + xOffset, y, drawWidth, height, 0.f, 0.f, drawWidth, textureSize.y);
			}
			return;
		}

		for (float yOffset = 0.f; yOffset < height; yOffset += textureSize.y)
		{
			const float drawHeight = std::min(textureSize.y, height - yOffset);
			Renderer::GetInstance().RenderTexture(texture, x, y + yOffset, width, drawHeight, 0.f, 0.f, textureSize.x, drawHeight);
		}
	}

	void TileMapComponent::RenderTunnelPreview() const
	{
		const auto& preview = LevelManager::GetInstance().GetTunnelPreview();
		if (!preview.active || preview.type == LevelObjectType::none || preview.progress <= 0.f)
		{
			return;
		}

		auto textureIt = m_tileTextures.find(preview.type);
		if (textureIt == m_tileTextures.end() || textureIt->second == nullptr)
		{
			return;
		}

		const float drawX = m_parent->GetWorldPosition().x + static_cast<float>(preview.cellX) * m_tileWidth;
		const float drawY = m_parent->GetWorldPosition().y + static_cast<float>(preview.cellY) * m_tileHeight;
		const float clampedProgress = std::clamp(preview.progress, 0.f, 1.f);

		if (preview.type == LevelObjectType::horizontalTunnel)
		{
			const float width = m_tileWidth * clampedProgress;
			if (width <= 0.f)
			{
				return;
			}

			const glm::vec2 textureSize = textureIt->second->GetSize();

			const float offset = preview.direction == TunnelDirection::left ? m_tileWidth - width : 0.f;
			for (float xOffset = 0.f; xOffset < width; xOffset += textureSize.x)
			{
				const float drawWidth = std::min(textureSize.x, width - xOffset);
				Renderer::GetInstance().RenderTexture(*textureIt->second, drawX + offset + xOffset, drawY, drawWidth, m_tileHeight, 0.f, 0.f, drawWidth, textureSize.y);
			}
			return;
		}

		if (preview.type == LevelObjectType::verticalTunnel)
		{
			const float height = m_tileHeight * clampedProgress;
			if (height <= 0.f)
			{
				return;
			}

			const glm::vec2 textureSize = textureIt->second->GetSize();

			const float offset = preview.direction == TunnelDirection::up ? m_tileHeight - height : 0.f;
			for (float yOffset = 0.f; yOffset < height; yOffset += textureSize.y)
			{
				const float drawHeight = std::min(textureSize.y, height - yOffset);
				Renderer::GetInstance().RenderTexture(*textureIt->second, drawX, drawY + offset + yOffset, m_tileWidth, drawHeight, 0.f, 0.f, textureSize.x, drawHeight);
			}
			return;
		}
	}

	LevelObjectType TileMapComponent::GetConnectedTunnelType(size_t x, size_t y) const
	{
		if (m_tiles == nullptr || y >= m_tiles->size() || x >= m_tiles->at(y).size() || m_tiles->at(y).at(x) != LevelObjectType::tunnelEnd)
		{
			return LevelObjectType::none;
		}

        const bool hasHorizontalConnection = (x > 0 && IsHorizontalConnector(m_tiles->at(y).at(x - 1))) || (x + 1 < m_tiles->at(y).size() && IsHorizontalConnector(m_tiles->at(y).at(x + 1)));
		if (hasHorizontalConnection)
		{
			return LevelObjectType::horizontalTunnel;
		}

       const bool hasVerticalConnection = (y > 0 && x < m_tiles->at(y - 1).size() && IsVerticalConnector(m_tiles->at(y - 1).at(x))) || (y + 1 < m_tiles->size() && x < m_tiles->at(y + 1).size() && IsVerticalConnector(m_tiles->at(y + 1).at(x)));
		if (hasVerticalConnection)
		{
			return LevelObjectType::verticalTunnel;
		}

		return LevelObjectType::none;
	}

	void TileMapComponent::SetTiles(const std::vector<std::vector<LevelObjectType>>* tiles)
	{
		m_tiles = tiles;
	}

	void TileMapComponent::SetTileTexture(LevelObjectType type, const std::string& filename)
	{
		m_tileTextures[type] = ResourceManager::GetInstance().LoadTexture(filename);
	}

	void TileMapComponent::SetTileSize(float tileWidth, float tileHeight)
	{
		m_tileWidth = tileWidth;
		m_tileHeight = tileHeight;
	}

	void TileMapComponent::Render() const
	{
		if (m_tiles == nullptr)
		{
			return;
		}

		const auto& origin = m_parent->GetWorldPosition();

		for (size_t y{}; y < m_tiles->size(); ++y)
		{
            for (size_t x{}; x < m_tiles->at(y).size(); ++x)
			{
                const auto type = m_tiles->at(y).at(x);
				if (type == LevelObjectType::empty || type == LevelObjectType::none)
				{
					continue;
				}

				auto textureIt = m_tileTextures.find(type);
				if (textureIt == m_tileTextures.end() || textureIt->second == nullptr)
				{
					continue;
				}

				const float drawX = origin.x + static_cast<float>(x) * m_tileWidth;
				const float drawY = origin.y + static_cast<float>(y) * m_tileHeight;

				if (type == LevelObjectType::horizontalTunnel || type == LevelObjectType::verticalTunnel)
				{
					RenderTiledTexture(*textureIt->second, drawX, drawY, m_tileWidth, m_tileHeight, type == LevelObjectType::horizontalTunnel);
					continue;
				}

				Renderer::GetInstance().RenderTexture(*textureIt->second, drawX, drawY, m_tileWidth, m_tileHeight);
			}
		}
		RenderTunnelPreview();
	}
}

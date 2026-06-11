#include "TextureComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include <algorithm>

dae::TextureComponent::TextureComponent(GameObject* pOwner)
	: Component(pOwner)
{
}

void dae::TextureComponent::Render() const
{
	if (m_texture == nullptr)
	{
		return;
	}

	const auto& pos = m_parent->GetWorldPosition();
	if (!m_isTiled or m_tileArea.x <= 0.f or m_tileArea.y <= 0.f)
	{
		if (m_flipHorizontal or m_flipVertical)
		{
			Renderer::GetInstance().RenderTextureFlipped(*m_texture, pos.x, pos.y, m_drawSize.x, m_drawSize.y, m_flipHorizontal, m_flipVertical);
			return;
		}
		else {
			Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y, m_drawSize.x, m_drawSize.y);
		}
		return;
	}

	const glm::vec2 textureSize = m_texture->GetSize();
	if (textureSize.x <= 0.f or textureSize.y <= 0.f)
	{
		return;
	}

	for (float yOffset = 0.f; yOffset < m_tileArea.y; yOffset += textureSize.y)
	{
		for (float xOffset = 0.f; xOffset < m_tileArea.x; xOffset += textureSize.x)
		{
			const float drawWidth = std::min(textureSize.x, m_tileArea.x - xOffset);
			const float drawHeight = std::min(textureSize.y, m_tileArea.y - yOffset);
			Renderer::GetInstance().RenderTexture(*m_texture, pos.x + xOffset, pos.y + yOffset, drawWidth, drawHeight, 0.f, 0.f, drawWidth, drawHeight);
		}
	}
}

void dae::TextureComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::TextureComponent::SetTiling(const bool isTiled)
{
	m_isTiled = isTiled;
}

void dae::TextureComponent::SetTileArea(const float width, const float height)
{
	m_tileArea.x = width;
	m_tileArea.y = height;
}

void dae::TextureComponent::SetDrawSize(const float width, const float height)
{
	m_drawSize.x = width;
	m_drawSize.y = height;
}

void dae::TextureComponent::FlipHorizontal(const bool flip)
{
	m_flipHorizontal = flip;
}

void dae::TextureComponent::FlipVertical(const bool flip)
{
	m_flipVertical = flip;
}
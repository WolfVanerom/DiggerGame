#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "TextComponent.h"
#include "Renderer.h"
#include "Font.h"
#include "Texture2D.h"

dae::TextComponent::TextComponent(GameObject* pOwner, const std::string& text, Font* font, const SDL_Color& color)
	: Component(pOwner), m_needsUpdate(true), m_text(text), m_color(color), m_font(font), m_textTexture(nullptr)
{
}

void dae::TextComponent::Update(float)
{
	if (m_needsUpdate)
	{
		const auto surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), 0, m_color);
		if (surf == nullptr) 
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}
		auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);
		if (texture == nullptr) 
		{
			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}
       if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR))
		{
			SDL_DestroySurface(surf);
			SDL_DestroyTexture(texture);
			throw std::runtime_error(std::string("Set text texture scale mode failed: ") + SDL_GetError());
		}
		SDL_DestroySurface(surf);
		m_textTexture = std::make_shared<Texture2D>(texture);
		m_needsUpdate = false;
	}
}

void dae::TextComponent::Render() const
{
	if (m_textTexture != nullptr)
	{
		const auto& pos = m_transform.GetWorldPosition();
        const auto textureSize = m_textTexture->GetSize();
		Renderer::GetInstance().RenderTexture(*m_textTexture, pos.x, pos.y, textureSize.x * m_scale, textureSize.y * m_scale);
	}
}

void dae::TextComponent::SetText(const std::string& text)
{
	m_text = text;
	m_needsUpdate = true;
}

void dae::TextComponent::SetPosition(const float x, const float y)
{
	m_transform.SetWorldPosition(x, y);
}

void dae::TextComponent::SetScale(const float scale)
{
	m_scale = scale;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{ 
	m_color = color; 
	m_needsUpdate = true; 
}

float dae::TextComponent::GetWidth() const
{
	auto textureSize = m_textTexture->GetSize();
	return textureSize.x * m_scale;
}

float dae::TextComponent::GetHeight() const
{
	auto textureSize = m_textTexture->GetSize();
	return textureSize.y * m_scale;
}
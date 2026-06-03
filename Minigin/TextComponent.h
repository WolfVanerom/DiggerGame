#pragma once
#include <string>
#include <memory>
#include <SDL3/SDL.h>
#include "Component.h"
#include "Transform.h"

namespace dae
{
	class Font;
	class Texture2D;
	class TextComponent final : public Component
	{
	public:
		void Update(float deltaTime) override;
		void Render() const override;

		void SetText(const std::string& text);
		void SetPosition(float x, float y);
		void SetScale(float scale);
		void SetColor(const SDL_Color& color);

		float GetWidth() const;
		float GetHeight() const;

		explicit TextComponent(GameObject* pOwner, const std::string& text, Font* font, const SDL_Color& color);
		virtual ~TextComponent() = default;
		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;
	private:
		bool m_needsUpdate{};
		std::string m_text{};
		SDL_Color m_color{ 255, 255, 255, 255 };
        float m_scale{ 1.0f };
		Transform m_transform{};
		Font* m_font{};
		std::shared_ptr<Texture2D> m_textTexture{};
	};
}

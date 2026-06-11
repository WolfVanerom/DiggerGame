#pragma once
#include "Component.h"
#include <glm/vec2.hpp>
#include <string>

namespace dae
{
	class Texture2D;
	class TextureComponent final : public Component
	{
	public:
		void Render() const override;

		void SetTexture(const std::string& filename);
		void SetTiling(bool isTiled);
		void SetTileArea(float width, float height);
		void SetDrawSize(float width, float height);
		void FlipHorizontal(bool flip);
		void FlipVertical(bool flip);

		float GetWidth() const { return m_drawSize.x; }
		float GetHeight() const { return m_drawSize.y; }

		explicit TextureComponent(GameObject* pOwner);
		virtual ~TextureComponent() = default;
		TextureComponent(const TextureComponent& other) = delete;
		TextureComponent(TextureComponent&& other) = delete;
		TextureComponent& operator=(const TextureComponent& other) = delete;
		TextureComponent& operator=(TextureComponent&& other) = delete;
	private:
		std::shared_ptr<Texture2D> m_texture{};
		bool m_isTiled{ false };
		glm::vec2 m_tileArea{};
		glm::vec2 m_drawSize{};
		bool m_flipHorizontal{ false };
		bool m_flipVertical{ false };
	};
}
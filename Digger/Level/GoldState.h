#pragma once

namespace dae {

	class GoldComponent;
	class TextureComponent;
	class GoldState
	{
	protected:
		GoldComponent* m_pGoldComponent{ nullptr };
	public:
		void SetGoldComponent(GoldComponent* pGoldComponent) { m_pGoldComponent = pGoldComponent; }
		virtual void SetState() = 0;
		virtual void Update(float deltaTime) = 0;
	};

	class IdleState final : public GoldState
	{
	public:
		void SetState() override;
		void Update(float deltaTime) override;
	};

	class FallingState final : public GoldState
	{
		void SetState() override;
		void Update(float deltaTime) override;
	};

	class BrokenState final : public GoldState
	{
	protected:
		TextureComponent* m_pTextureComponent{ nullptr };
		void SetState() override;
		void Update(float deltaTime) override;
	public:
		void SetTextureComponent(TextureComponent* textureComponent);
	};
}
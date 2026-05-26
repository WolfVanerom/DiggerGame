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
		virtual void OnEnter() = 0;
		virtual void OnExit() = 0;
		virtual void Update(float deltaTime) = 0;
	};

	class IdleState final : public GoldState
	{
	protected:
		TextureComponent* m_pTextureComponent{ nullptr };
	public:
		void OnEnter() override;
		void OnExit() override;
		void Update(float deltaTime) override;
		void SetTextureComponent(TextureComponent* textureComponent);
	};

	class FallingState final : public GoldState
	{
	protected:
		int m_AmountFallen{ 0 };
		int m_PrevCellY{ -1 };
	public:
		void OnEnter() override;
		void OnExit() override;
		void Update(float deltaTime) override;
	};

	class BrokenState final : public GoldState
	{
	protected:
		TextureComponent* m_pTextureComponent{ nullptr };
	public:
		void OnEnter() override;
		void OnExit() override;
		void Update(float deltaTime) override;
		void SetTextureComponent(TextureComponent* textureComponent);
	};
}
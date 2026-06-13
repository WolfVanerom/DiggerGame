#pragma once
#include <Component.h>
#include <vector>
#include <TextComponent.h>

namespace dae
{
	class LetterSwitchComponent final : public Component
	{
	public:
		LetterSwitchComponent(GameObject* pOwner, char initialLetter = 'A', TextComponent* pTextComponent = nullptr);
		LetterSwitchComponent(const LetterSwitchComponent& other) = delete;
		LetterSwitchComponent(LetterSwitchComponent&& other) = delete;
		LetterSwitchComponent& operator=(const LetterSwitchComponent& other) = delete;
		LetterSwitchComponent& operator=(LetterSwitchComponent&& other) = delete;
		virtual ~LetterSwitchComponent() = default;
		char GetLetter() const;
		void NextLetter();
		void PreviousLetter();
	private:
		char m_Letter;
		const std::vector<char> m_Alphabet{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
		TextComponent* m_pTextComponent;
	};
}
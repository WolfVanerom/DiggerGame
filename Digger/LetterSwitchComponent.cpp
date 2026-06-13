#include "LetterSwitchComponent.h"

namespace dae
{
	LetterSwitchComponent::LetterSwitchComponent(GameObject* pOwner, char initialLetter, TextComponent* pTextComponent)
		: Component(pOwner)
		, m_Letter(initialLetter)
		, m_pTextComponent(pTextComponent)
	{
		if (m_pTextComponent)
		{
			m_pTextComponent->SetText(std::string(1, m_Letter));
		}
	}

	char LetterSwitchComponent::GetLetter() const
	{
		return m_Letter;
	}

	void LetterSwitchComponent::NextLetter()
	{
		auto it = std::find(m_Alphabet.begin(), m_Alphabet.end(), m_Letter);
		if (it != m_Alphabet.end())
		{
			it = std::next(it);
			if (it == m_Alphabet.end())
			{
				it = m_Alphabet.begin();
			}
			m_Letter = *it;
			if (m_pTextComponent)
			{
				m_pTextComponent->SetText(std::string(1, m_Letter));
			}
		}
	}

	void LetterSwitchComponent::PreviousLetter()
	{
		auto it = std::find(m_Alphabet.begin(), m_Alphabet.end(), m_Letter);
		if (it == m_Alphabet.begin())
		{
			return;
		}
		if (it != m_Alphabet.end())
		{
			it = std::prev(it);
			if (it == m_Alphabet.end())
			{
				it = m_Alphabet.end();
			}
			m_Letter = *it;
			if (m_pTextComponent)
			{
				m_pTextComponent->SetText(std::string(1, m_Letter));
			}
		}
	}
}
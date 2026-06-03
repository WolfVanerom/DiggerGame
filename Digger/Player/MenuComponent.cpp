#include "MenuComponent.h"
#include "ButtonComponent.h"

namespace dae
{
	MenuComponent::MenuComponent(GameObject* pOwner, Scene* pScene)
		: Component(pOwner), m_pScene(pScene)
	{
	}

	void MenuComponent::MoveRight()
	{
		for (size_t row = 0; row < m_pMenuItems.size(); ++row)
		{
			auto it = std::find(m_pMenuItems[row].begin(), m_pMenuItems[row].end(), m_pSelectedItem);
			if (it != m_pMenuItems[row].end())
			{
				size_t col = std::distance(m_pMenuItems[row].begin(), it);
				col = (col + 1) % m_pMenuItems[row].size();
				m_pSelectedItem->Unhighlight();
				m_pSelectedItem = m_pMenuItems[row][col];
				HighlightSelectedItem();
				return;
			}
		}
	}

	void MenuComponent::MoveLeft()
	{
		for (size_t row = 0; row < m_pMenuItems.size(); ++row)
		{
			auto it = std::find(m_pMenuItems[row].begin(), m_pMenuItems[row].end(), m_pSelectedItem);
			if (it != m_pMenuItems[row].end())
			{
				size_t col = std::distance(m_pMenuItems[row].begin(), it);
				col = (col - 1 + m_pMenuItems[row].size()) % m_pMenuItems[row].size();
				m_pSelectedItem->Unhighlight();
				m_pSelectedItem = m_pMenuItems[row][col];
				HighlightSelectedItem();
				return;
			}
		}
	}

	void MenuComponent::MoveUp()
	{
		for (size_t row = 0; row < m_pMenuItems.size(); ++row)
		{
			auto it = std::find(m_pMenuItems[row].begin(), m_pMenuItems[row].end(), m_pSelectedItem);
			if (it != m_pMenuItems[row].end())
			{
				size_t col = std::distance(m_pMenuItems[row].begin(), it);
				size_t targetRow = (row - 1 + m_pMenuItems.size()) % m_pMenuItems.size();
				if (m_pMenuItems[targetRow].empty()) {
					return;
				}
				if (col >= m_pMenuItems[targetRow].size()) {
					col = m_pMenuItems[targetRow].size() - 1;
				}
				m_pSelectedItem->Unhighlight();
				m_pSelectedItem = m_pMenuItems[targetRow][col];
				HighlightSelectedItem();
				return;
			}
		}
	}

	void MenuComponent::MoveDown()
	{
		for (size_t row = 0; row < m_pMenuItems.size(); ++row)
		{
			auto it = std::find(m_pMenuItems[row].begin(), m_pMenuItems[row].end(), m_pSelectedItem);
			if (it != m_pMenuItems[row].end())
			{
				size_t col = std::distance(m_pMenuItems[row].begin(), it);
				size_t targetRow = (row + 1) % m_pMenuItems.size();
				if (m_pMenuItems[targetRow].empty()) {
					return;
				}
				if (col >= m_pMenuItems[targetRow].size()) {
					col = m_pMenuItems[targetRow].size() - 1;
				}
				m_pSelectedItem->Unhighlight();
				m_pSelectedItem = m_pMenuItems[targetRow][col];
				HighlightSelectedItem();
				return;
			}
		}
	}

	void MenuComponent::Update(float)
	{
	}

	void MenuComponent::AddMenuItem(ButtonComponent* pButton, unsigned int column)
	{
		if (column >= m_pMenuItems.size())
		{
			m_pMenuItems.resize(column + 1);
		}
		m_pMenuItems[column].push_back(pButton);
	}

	void MenuComponent::SetSelectedItem(ButtonComponent* pButton)
	{
		m_pSelectedItem = pButton;
		HighlightSelectedItem();
	}

	void MenuComponent::ClickSelectedItem()
	{
		if (m_pSelectedItem)
		{
			m_pSelectedItem->OnClick();
		}
	}

	void MenuComponent::HighlightSelectedItem()
	{
		m_pSelectedItem->Highlight();
	}
}
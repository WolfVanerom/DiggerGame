#pragma once
#include "GameObject.h"
#include <algorithm>
#include <vector>

namespace dae {

	enum class Event
	{
		None,
		RemainingLivesChanged,
		PlayerDied,
		EnemyDefeated,
		ScoreChanged,
		LevelCompleted
	};

	struct Observer
	{
		virtual ~Observer() = default;
		virtual void OnNotify(Event event, GameObject* gameObject) = 0;
	};

	class subject
	{
	public:
		virtual void AddObserver(Observer* observer)
		{
			m_observers.push_back(observer);
		}
		virtual void RemoveObserver(Observer* observer)
		{
			m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
		}
		virtual ~subject()=0;
	protected:
		virtual void Notify(Event event, GameObject* gameObject)
		{
			for (Observer* observer : m_observers)
			{
				observer->OnNotify(event, gameObject);
			}
		}
	private:
		std::vector<Observer*> m_observers;
	};

	class TextComponent;
	class PlayerComponent;
}
#include "Observer.h"

namespace dae
{
    subject::~subject() = default;

	void subject::Notify(Event event, GameObject* gameObject)
	{
		for (Observer* observer : m_observers)
		{
			observer->OnNotify(event, gameObject);
		}
	}
}

#pragma once
#include <glm/glm.hpp>


namespace dae
{
	class Transform final
	{
	public:
		const glm::vec3& GetLocalPosition() const;
		const glm::vec3& GetWorldPosition() const;
		void SetLocalPosition(float x, float y, float z = 0);
		void SetLocalPosition(const glm::vec3& position);
		void SetWorldPosition(float x, float y, float z = 0);
		void SetWorldPosition(const glm::vec3& position);
	private:
		glm::vec3 m_localPosition;
		glm::vec3 m_worldPosition;
	};
}

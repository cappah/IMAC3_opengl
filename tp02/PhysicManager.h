#pragma once

#include "Flag.h"

namespace Physic {
	class PhysicManager
	{
	private:
		glm::vec3 m_gravity;

	public:
		PhysicManager();
		~PhysicManager();

		void setGravity(const glm::vec3& g);
		glm::vec3 getGravity() const;

		void update(float deltaTime, std::vector<Flag*>& flags);
	};
}


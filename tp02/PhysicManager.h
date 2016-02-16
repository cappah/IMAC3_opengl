#pragma once

#include "Flag.h"
#include "WindZone.h"

namespace Physic {
	class PhysicManager
	{
	private:
		glm::vec3 m_gravity;

	public:
		PhysicManager(const glm::vec3& _gravity = glm::vec3(0.f,-9.8f,0.f));
		~PhysicManager();

		void setGravity(const glm::vec3& g);
		glm::vec3 getGravity() const;

		void update(float deltaTime, std::vector<Flag*>& flags, std::vector<WindZone*>& windZones);
	};
}


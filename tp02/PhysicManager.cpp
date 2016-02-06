#include "PhysicManager.h"

namespace Physic {

	PhysicManager::PhysicManager(const glm::vec3& _gravity) : m_gravity(_gravity)
	{
	}


	PhysicManager::~PhysicManager()
	{
	}

	void PhysicManager::setGravity(const glm::vec3&  g)
	{
		m_gravity = g;
	}

	glm::vec3  PhysicManager::getGravity() const
	{
		return m_gravity;
	}

	void PhysicManager::update(float deltaTime, std::vector<Flag*>& flags)
	{
		for (int i = 0; i < flags.size(); i++)
		{
			flags[i]->applyGravity(m_gravity);
			flags[i]->update(deltaTime);
		}
	}

}

#include "PhysicManager.h"
#include "Application.h"

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

	void PhysicManager::update(float deltaTime, std::vector<Flag*>& flags, std::vector<WindZone*>& windZones)
	{
		for (int i = 0; i < flags.size(); i++)
		{
			for (int j = 0; j < windZones.size(); j++)
			{
				flags[i]->applyForce(windZones[i]->getForce(Application::get().getTime()));
			}

			flags[i]->applyGravity(m_gravity);
			flags[i]->update(deltaTime);
		}
	}

}

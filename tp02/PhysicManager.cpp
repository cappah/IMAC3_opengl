#include "PhysicManager.h"
#include "Application.h"
#include "Entity.h" // TODO remove 

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

	void PhysicManager::update(float deltaTime, const BaseCamera& camera, std::vector<Flag*>& flags, Terrain& terrain, std::vector<WindZone*>& windZones, std::vector<ParticleEmitter*>& particleEmitters)
	{
		for (int i = 0; i < flags.size(); i++)
		{
			for (int j = 0; j < windZones.size(); j++)
			{
				// TODO replace by flags[i]->getTransform().position...
				flags[i]->applyForce(windZones[j]->getForce(Application::get().getTime(), flags[i]->entity()->getTranslation() ));
			}

			flags[i]->applyGravity(m_gravity);
			flags[i]->update(deltaTime);
		}

		//update terrain : 
		terrain.updatePhysic(deltaTime, windZones);

		//update particles : 
		for (int i = 0; i < particleEmitters.size(); i++)
		{
			particleEmitters[i]->update(deltaTime, camera.getCameraPosition());
		}
	}

}

#include "PhysicManager.h"
#include "Application.h"
#include "Entity.h" // TODO remove 

namespace Physic {


	void computeLinks(float deltaTime, Link* link)
	{
		float d = glm::distance(link->M1->position, link->M2->position);
		if (d < 0.00000001f)
			return;

		float f = link->k * (1.f - link->l / d);
		if (std::abs(f) < 0.00000001f)
			return;

		glm::vec3 M1M2 = link->M2->position - link->M1->position;
		glm::normalize(M1M2);
		if (glm::length(M1M2) < 0.00000001f)
			return;

		//frein :
		glm::vec3 frein = link->z*(link->M2->vitesse - link->M1->vitesse);

		link->M1->force += (f * M1M2 + frein);
		link->M2->force += (-f * M1M2 - frein);

	}

	void computePoints(float deltaTime, Point* point)
	{
		if (point->masse < 0.00000001f)
			return;

		//leapfrog
		point->vitesse += (deltaTime / point->masse)*point->force;
		point->position += deltaTime*point->vitesse;
		point->force = glm::vec3(0, 0, 0);
	}


	///////////////////////////////

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

	void PhysicManager::update(float deltaTime, std::vector<Flag*>& flags, Terrain& terrain, std::vector<WindZone*>& windZones)
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

			//update terrain : 
			terrain.updatePhysic(windZones);
		}
	}

}

#include "PhysicManager.h"

namespace Physic {

	PhysicManager::PhysicManager()
	{
	}


	PhysicManager::~PhysicManager()
	{
	}

	void PhysicManager::update(float deltaTime, std::vector<Flag*>& flags)
	{
		for (int i = 0; i < flags.size(); i++)
		{
			flags[i]->update(deltaTime);
		}
	}

}

#pragma once

#include "Flag.h"

namespace Physic {
	class PhysicManager
	{
	public:
		PhysicManager();
		~PhysicManager();

		void update(float deltaTime, std::vector<Flag*>& flags);
	};
}


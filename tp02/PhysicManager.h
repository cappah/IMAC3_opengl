#pragma once

#include "Flag.h"
#include "WindZone.h"
#include "Terrain.h"
#include "ParticleEmitter.h"
#include "Camera.h"

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

		void update(float deltaTime, const BaseCamera& camera, std::vector<Flag*>& flags, Terrain& terrain, std::vector<WindZone*>& windZones, std::vector<ParticleEmitter*>& particleEmitters);
	};
}


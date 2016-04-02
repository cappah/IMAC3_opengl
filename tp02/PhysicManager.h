#pragma once

#include "Flag.h"
#include "WindZone.h"
#include "Terrain.h"
#include "ParticleEmitter.h"
#include "Camera.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "linearMath/btIDebugDraw.h"

namespace Physic {

	class DebugDrawerPhysicWorld : public btIDebugDraw
	{
	private:
		int m_debugMode;

	public:
		DebugDrawerPhysicWorld();
		virtual ~DebugDrawerPhysicWorld();

		void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
		void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
		void reportErrorWarning(const char *warningString);
		void draw3dText(const btVector3 &location, const char *textString);
		void setDebugMode(int debugMode);
		int getDebugMode() const;
	};
	
	class PhysicManager
	{
	private:
		glm::vec3 m_gravity;

		btDefaultCollisionConfiguration* m_collisionConfiguration;
		btCollisionDispatcher* m_dispatcher;
		btDbvtBroadphase* m_broadPhase;
		btSequentialImpulseConstraintSolver* m_sequentialImpulseConstraintSolver;

		btDiscreteDynamicsWorld* m_physicWorld;

		DebugDrawerPhysicWorld* m_debugDrawerPhysicWorld;

	public:
		PhysicManager(const glm::vec3& _gravity = glm::vec3(0.f,-9.8f,0.f));
		~PhysicManager();

		void setGravity(const glm::vec3& g);
		glm::vec3 getGravity() const;

		btDiscreteDynamicsWorld* getBulletDynamicSimulation() const;

		void update(float deltaTime, const BaseCamera& camera, std::vector<Flag*>& flags, Terrain& terrain, std::vector<WindZone*>& windZones, std::vector<ParticleEmitter*>& particleEmitters);
		void update(float deltaTime, const BaseCamera& camera, std::vector<Flag*>& flags, Terrain& terrain, std::vector<WindZone*>& windZones, std::vector<ParticleEmitter*>& particleEmitters, bool updateRigidbodies);
	
		void debugDraw(const glm::mat4& projection, const glm::mat4& view) const;
	};
}


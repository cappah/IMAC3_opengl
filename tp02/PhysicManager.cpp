#include "PhysicManager.h"
//forwards :
#include "Application.h"
#include "Entity.h" // TODO remove 
#include "DebugDrawer.h"

namespace Physic {


	DebugDrawerPhysicWorld::DebugDrawerPhysicWorld() : m_debugMode(DBG_DrawWireframe | DBG_DrawAabb)
	{
	}

	DebugDrawerPhysicWorld::~DebugDrawerPhysicWorld()
	{

	}

	void DebugDrawerPhysicWorld::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
	{
		DebugDrawer::drawLine(glm::vec3(from.x(), from.y(), from.z()), glm::vec3(to.x(), to.y(), to.z()), glm::vec3(color.x(), color.y(), color.z()));
	}

	void DebugDrawerPhysicWorld::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
	{
		//nothing
	}

	void DebugDrawerPhysicWorld::reportErrorWarning(const char *warningString)
	{
		std::cerr << warningString << std::endl;
	}

	void DebugDrawerPhysicWorld::draw3dText(const btVector3 &location, const char *textString)
	{
		//nothing
	}

	void DebugDrawerPhysicWorld::setDebugMode(int debugMode)
	{
		m_debugMode = debugMode;
	}

	int DebugDrawerPhysicWorld::getDebugMode() const
	{
		return m_debugMode;
	}

	//////////////////////////////////////////////////


	PhysicManager::PhysicManager(const glm::vec3& _gravity) : m_gravity(_gravity)
	{
		//build the bullet physic world : 
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
		m_broadPhase = new btDbvtBroadphase();
		m_sequentialImpulseConstraintSolver = new btSequentialImpulseConstraintSolver();

		m_physicWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadPhase, m_sequentialImpulseConstraintSolver, m_collisionConfiguration);

		// debug :
		m_debugDrawerPhysicWorld = new DebugDrawerPhysicWorld();

		btIDebugDraw::DefaultColors debugColors;
		debugColors.m_aabb = btVector3(1, 0, 0);
		debugColors.m_activeObject = btVector3(0, 1, 0);
		debugColors.m_deactivatedObject = btVector3(0, 0, 1);

		m_debugDrawerPhysicWorld->setDefaultColors(debugColors);
		m_debugDrawerPhysicWorld->setDebugMode(btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawWireframe);
		m_physicWorld->setDebugDrawer(m_debugDrawerPhysicWorld);
	}


	PhysicManager::~PhysicManager()
	{
		delete m_physicWorld;

		delete m_sequentialImpulseConstraintSolver;
		delete m_broadPhase;
		delete m_dispatcher;
		delete m_collisionConfiguration;

		// debug :
		delete m_debugDrawerPhysicWorld;
	}

	void PhysicManager::setGravity(const glm::vec3&  g)
	{
		m_gravity = g;
		m_physicWorld->setGravity(btVector3(m_gravity.x, m_gravity.y, m_gravity.z));
	}

	glm::vec3  PhysicManager::getGravity() const
	{
		return m_gravity;
	}

	btDiscreteDynamicsWorld * PhysicManager::getBulletDynamicSimulation() const
	{
		return m_physicWorld;
	}

	void PhysicManager::update(float deltaTime, const BaseCamera& camera, std::vector<Flag*>& flags, Terrain& terrain, std::vector<WindZone*>& windZones, std::vector<ParticleEmitter*>& particleEmitters)
	{
		//update bullet internal physic :
		m_physicWorld->stepSimulation(Application::get().getFixedDeltaTime(), 10);

		//update the reste of physic : 

		//update flags :
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


	void PhysicManager::update(float deltaTime, const BaseCamera& camera, std::vector<Flag*>& flags, Terrain& terrain, std::vector<WindZone*>& windZones, std::vector<ParticleEmitter*>& particleEmitters, bool updateRigidbodies)
	{
		if (updateRigidbodies) {
			//update bullet internal physic :
			m_physicWorld->stepSimulation(Application::get().getFixedDeltaTime(), 10);
		}

		//update flags :
		for (int i = 0; i < flags.size(); i++)
		{
			for (int j = 0; j < windZones.size(); j++)
			{
				// TODO replace by flags[i]->getTransform().position...
				flags[i]->applyForce(windZones[j]->getForce(Application::get().getTime(), flags[i]->entity()->getTranslation()));
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

	void PhysicManager::debugDraw(const glm::mat4 & projection, const glm::mat4 & view) const
	{
		if (m_debugDrawerPhysicWorld != nullptr) {
			m_physicWorld->debugDrawWorld();
		}
	}

}

#pragma once

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "MotionState.h"
#include "Component.h"
#include "Collider.h"

class Rigidbody : public Component
{
private:
	glm::vec3 m_translation;
	glm::quat m_rotation;
	glm::vec3 m_scale;

	btScalar m_mass;
	btVector3 m_inertia;

	btRigidBody* m_bulletRigidbody;

	MotionState* m_motionState;
	btCompoundShape* m_shape;
	std::vector<Collider*> m_colliders;

	btDiscreteDynamicsWorld* m_ptrToPhysicWorld;

public:
	Rigidbody();
	~Rigidbody();

    /**
     * build a bullet rigidbody and send it to the btDynamicWorld
     * if m_target is in wolrd do nothing
     * to update a rigidbody, call popFromSimulation() and then pushToSimulation();
     */
    void pushToSimulation();

    /**
     * remove the target btRigidbody (if any) from the btDynamicWorld
     */
    void popFromSimulation();

    /**
     * delete the previous shape (if any), then make the multi shape, base on all the collider attached to this entity
     */
    void makeShape();

    /**
	 * Must be called after the rigidbody has been attached to an entity.
     * calculate mass, inertia, and motion state, then create appropriate btRigidBody. and place it in m_bulletRigidbody parameter
	 * also set the m_ptrToPhysicWorld with physicSimulation.
     */
    void init(btDiscreteDynamicsWorld* physicSimulation);

    float getMass() const;
    void setMass(float mass);
    btVector3 getInertia() const;
    void setInertia(const glm::vec3 &inertia);
	void setPtrToPhysicWorld(btDiscreteDynamicsWorld* ptrToPhysicWorld);
	void addColliders(std::vector<Collider*> colliders);
	void addCollider(Collider* collider);
	//only remove a collider from the rigidbody, don't destroy them.
	void removeCollider(Collider* collider);
	//only remove all colliders from the rigidbody, don't destroy them.
	void removeAllColliders();
	//update the shape of the collider : 


	virtual void drawUI(Scene& scene) override;

	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1, 1, 1), const glm::quat& rotation = glm::quat());
	void applyTransformFromPhysicSimulation();

	virtual void eraseFromScene(Scene& scene) override;
	virtual void addToScene(Scene& scene) override;

	virtual void eraseFromEntity(Entity& entity) override;
	virtual void addToEntity(Entity& entity) override;

	virtual Component* clone(Entity* entity) override;

	virtual void save(Json::Value& componentRoot) const override;
	virtual void load(Json::Value& componentRoot) override;
};

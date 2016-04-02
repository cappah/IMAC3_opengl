#include "Rigidbody.h"
//forwards : 
#include "Scene.h"



Rigidbody::Rigidbody() : Component(ComponentType::RIGIDBODY),
m_translation(0,0,0), m_scale(1,1,1), m_mass(0), m_inertia(0,0,0), 
m_bulletRigidbody(nullptr), m_motionState(nullptr), m_shape(nullptr), m_ptrToPhysicWorld(nullptr)
{

}


Rigidbody::~Rigidbody()
{
	popFromSimulation();

	delete m_bulletRigidbody;
	delete m_motionState;
	delete m_shape;
}

void Rigidbody::pushToSimulation()
{
	//target already pushed to world
	if (m_bulletRigidbody != nullptr && m_bulletRigidbody->isInWorld())
		return;

	//add it to the world :
	m_ptrToPhysicWorld->addRigidBody(m_bulletRigidbody);
}

void Rigidbody::popFromSimulation()
{
	//remove btRigidBody from the world :
	if (m_bulletRigidbody->isInWorld())
	{
		m_ptrToPhysicWorld->removeRigidBody(m_bulletRigidbody);
	}
}

void Rigidbody::makeShape()
{
	//make a shape
	std::vector<Collider*> colliders = getComponents<Collider>(ComponentType::COLLIDER);

	if (m_shape != nullptr)
		delete m_shape;

	m_shape = new btCompoundShape();

	for (auto collider : colliders)
	{
		btCollisionShape* childShape = collider->makeShape();
		btTransform childTransform;
		childTransform.setIdentity();
		childTransform.setFromOpenGLMatrix(glm::value_ptr(collider->getOffsetMatrix()));

		m_shape->addChildShape(childTransform, childShape);
	}

	//calculate mass, motion state and inertia :
	if (m_mass != 0)
		m_shape->calculateLocalInertia(m_mass, m_inertia);
	
	//if this collider already is in the simulation, update it : 
	if (m_bulletRigidbody != nullptr) {
		popFromSimulation();
			m_bulletRigidbody->setCollisionShape(m_shape);
		pushToSimulation();
	}
}

void Rigidbody::init(btDiscreteDynamicsWorld* physicSimulation)
{
	m_ptrToPhysicWorld = physicSimulation;

	//calculate mass, motion state and inertia :
	if (m_mass != 0)
		m_shape->calculateLocalInertia(m_mass, m_inertia);

	if (m_motionState != nullptr)
		delete m_motionState;
	m_motionState = new MotionState(entity());

	btRigidBody::btRigidBodyConstructionInfo constructorInfo(m_mass, m_motionState, m_shape, m_inertia);

	//create a btRigidBody base on the previous infos :
	if (m_bulletRigidbody != nullptr)
		delete m_bulletRigidbody;
	m_bulletRigidbody = new btRigidBody(constructorInfo);

	//if the rigidbody already has a collider shape, set it to the rigidbody and push rigidbody to simulation
	if (m_shape != nullptr) {
		popFromSimulation();
			m_bulletRigidbody->setCollisionShape(m_shape);
	}
	pushToSimulation();
}

float Rigidbody::getMass() const
{
	return m_mass;
}

void Rigidbody::setMass(float mass)
{
	m_mass = mass;

	popFromSimulation();
		btVector3 newInertia;
		m_bulletRigidbody->getCollisionShape()->calculateLocalInertia(m_mass, newInertia);
		m_bulletRigidbody->setMassProps(m_mass, newInertia);
		m_bulletRigidbody->updateInertiaTensor();
		//m_target->forceActivationState(DISABLE_DEACTIVATION);
		m_bulletRigidbody->activate(true);
	pushToSimulation();
}

btVector3 Rigidbody::getInertia() const
{
	return m_inertia;
}

void Rigidbody::setInertia(const glm::vec3 & inertia)
{
	m_inertia.setX(inertia.x);
	m_inertia.setY(inertia.y);
	m_inertia.setZ(inertia.z);
}

void Rigidbody::setPtrToPhysicWorld(btDiscreteDynamicsWorld * ptrToPhysicWorld)
{
	m_ptrToPhysicWorld = ptrToPhysicWorld;
}

void Rigidbody::addColliders(std::vector<Collider*> colliders)
{
	m_colliders.insert(m_colliders.begin(), colliders.begin(), colliders.end());
}

void Rigidbody::addCollider(Collider * collider)
{
	m_colliders.push_back(collider);
}

void Rigidbody::removeCollider(Collider * collider)
{
	m_colliders.erase(std::remove(m_colliders.begin(), m_colliders.end(), collider), m_colliders.end());
}

void Rigidbody::removeAllColliders()
{
	m_colliders.clear();
}

void Rigidbody::drawUI(Scene & scene)
{
	float tmpMass = m_mass;
	if (ImGui::InputFloat("mass", &tmpMass)) {
		setMass(tmpMass);
	}
}

void Rigidbody::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
{
	//update directly the rigidbody with the entity's transform
	btTransform newPhysicTransform;
	newPhysicTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	newPhysicTransform.setOrigin(btVector3(translation.x, translation.y, translation.z));

	//    if(m_target && m_target->isInWorld())
	//    {
	//        popFromSimulation();
	//    }

	m_bulletRigidbody->setWorldTransform(newPhysicTransform); // ???

	//scale :
	if (glm::distance(m_scale, scale) > 0.01f) {
		m_bulletRigidbody->getCollisionShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		btVector3 newInertia;
		m_bulletRigidbody->getCollisionShape()->calculateLocalInertia(m_mass, newInertia);
		m_bulletRigidbody->setMassProps(m_mass, newInertia);
		m_bulletRigidbody->updateInertiaTensor();
	}

	//add target to simulation :
	//pushToSimulation();

	if(m_bulletRigidbody->isKinematicObject())
		m_ptrToPhysicWorld->updateSingleAabb(m_bulletRigidbody);
	else {
		popFromSimulation();
		pushToSimulation();
	}


	m_translation = translation;
	m_rotation = rotation;
	m_scale = scale;
}

void Rigidbody::applyTransformFromPhysicSimulation(const glm::vec3 & translation, const glm::quat & rotation)
{
	m_translation = translation;
	m_rotation = rotation;
}

void Rigidbody::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

void Rigidbody::addToScene(Scene & scene)
{
	scene.add(this);
}

void Rigidbody::eraseFromEntity(Entity & entity)
{
	entity.erase(this);
}

void Rigidbody::addToEntity(Entity & entity)
{
	entity.add(this);
}

Component * Rigidbody::clone(Entity * entity)
{
	Rigidbody* newRigidbody = new Rigidbody(*this);

	newRigidbody->attachToEntity(entity);

	return newRigidbody;
}

void Rigidbody::save(Json::Value & componentRoot) const
{
	Component::save(componentRoot);

	componentRoot["translation"] = toJsonValue(m_translation);
	componentRoot["rotation"] = toJsonValue(m_rotation);
	componentRoot["scale"] = toJsonValue(m_scale);

	componentRoot["mass"] = m_mass*BT_ONE;
	componentRoot["inertia"] = toJsonValue(glm::vec3(m_inertia.x(), m_inertia.y(), m_inertia.z()));
}

void Rigidbody::load(Json::Value & componentRoot)
{
	Component::load(componentRoot);

	m_translation = glm::vec3(0, 0, 0); // fromJsonValue<glm::vec3>(componentRoot["translation"], glm::vec3(0, 0, 0));
	m_rotation = glm::quat(); // fromJsonValue<glm::quat>(componentRoot["rotation"], glm::quat());
	m_scale = glm::vec3(1, 1, 1); //fromJsonValue<glm::vec3>(componentRoot["scale"], glm::vec3(1,1,1));

	m_mass = btScalar(componentRoot.get("mass", 0.0f).asFloat());
	glm::vec3 tmpInertia = fromJsonValue<glm::vec3>(componentRoot["inertia"], glm::vec3(0,0,0));
	m_inertia = btVector3(tmpInertia.x, tmpInertia.y, tmpInertia.z);

	m_bulletRigidbody = nullptr;
	m_motionState = nullptr;
	m_shape = nullptr;
	m_ptrToPhysicWorld = nullptr;

	//initialization made when the rigidbody is attached to the entity.

}


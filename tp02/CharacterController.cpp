#include "CharacterController.h"
//forwards :
#include "Scene.h"
#include "Entity.h"

CharacterController::CharacterController(): Component(ComponentType::CHARACTER_CONTROLLER),
m_translation(0, 0, 0), m_scale(1, 1, 1), m_height(2.f), m_radius(0.5f), m_jumpFactor(1.f), m_gravityFactor(1.f),
m_isJumping(false), m_isOnGround(true), m_speed(0,0,0), m_force(0,0,0),
m_bulletGhostbody(nullptr), m_shape(nullptr), m_ptrToPhysicWorld(nullptr), m_isGhostInWorld(false)
{
	for (int i = 0; i < 4; i++) {
		m_speedFactor[i] = 1.f;
	}
}

CharacterController::CharacterController(const CharacterController & other) : Component(other),
m_translation(other.m_translation), m_scale(other.m_scale), m_height(other.m_height), m_radius(other.m_radius), m_jumpFactor(other.m_jumpFactor), m_gravityFactor(other.m_gravityFactor),
m_isJumping(other.m_isJumping), m_isOnGround(other.m_isOnGround), m_speed(other.m_speed), m_force(other.m_force),
m_bulletGhostbody(nullptr), m_shape(nullptr), m_ptrToPhysicWorld(other.m_ptrToPhysicWorld), m_isGhostInWorld(false)
{
	for (int i = 0; i < 4; i++) {
		m_speedFactor[i] = other.m_speedFactor[i];
	}
}

CharacterController & CharacterController::operator=(const CharacterController & other)
{
	Component::operator=(other);
	m_translation = other.m_translation;
	m_scale = other.m_scale;
	m_height = other.m_height;
	m_radius = other.m_radius;
	m_jumpFactor = other.m_jumpFactor;
	m_gravityFactor = other.m_gravityFactor;
	m_isJumping = false;
	m_isOnGround = other.m_isOnGround;
	m_speed = other.m_speed;
	m_force = other.m_force;

	m_isGhostInWorld = false;
	m_bulletGhostbody = nullptr;
	m_shape = nullptr;
	m_ptrToPhysicWorld = other.m_ptrToPhysicWorld;

		for (int i = 0; i < 4; i++) {
		m_speedFactor[i] = other.m_speedFactor[i];
	}
}

CharacterController::~CharacterController()
{
	popFromSimulation();

	delete m_bulletGhostbody;
	delete m_shape;
	m_bulletGhostbody = nullptr;
	m_shape = nullptr;
	m_ptrToPhysicWorld = nullptr;
}

void CharacterController::pushToSimulation()
{
	if (m_bulletGhostbody != nullptr && !m_isGhostInWorld)
		m_ptrToPhysicWorld->addCollisionObject(m_bulletGhostbody);
}

void CharacterController::popFromSimulation()
{
	//remove btRigidBody from the world :
	if (m_bulletGhostbody != nullptr && m_isGhostInWorld)
		m_ptrToPhysicWorld->removeCollisionObject(m_bulletGhostbody);
}

void CharacterController::makeShape()
{

	if (m_shape != nullptr)
		delete m_shape;

	m_shape = new btCapsuleShape(m_radius, m_height);

	//if this collider already is in the simulation, update it : 
	if (m_bulletGhostbody != nullptr) {
		popFromSimulation();
		m_bulletGhostbody->setCollisionShape(m_shape);
		pushToSimulation();
	}
}

void CharacterController::init(btDiscreteDynamicsWorld* physicSimulation)
{
	m_ptrToPhysicWorld = physicSimulation;
	//call it in case rigidbody or ghost object already is in world : 
	popFromSimulation();

	//create a btRigidBody base on the previous infos :
	if (m_bulletGhostbody != nullptr)
		delete m_bulletGhostbody;
	m_bulletGhostbody = new btPairCachingGhostObject();
	m_bulletGhostbody->setUserPointer(this);
	m_bulletGhostbody->setUserIndex(2); //TODO

	pushToSimulation();
}

void CharacterController::setPtrToPhysicWorld(btDiscreteDynamicsWorld * ptrToPhysicWorld)
{
	m_ptrToPhysicWorld = ptrToPhysicWorld;
}


void CharacterController::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
{	
	//update directly the rigidbody with the entity's transform
	btTransform newPhysicTransform;
	newPhysicTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	newPhysicTransform.setOrigin(btVector3(translation.x, translation.y, translation.z));

	m_bulletGhostbody->setWorldTransform(newPhysicTransform);

	 //scale :
	if (glm::distance(m_scale, scale) > 0.01f) {
		m_bulletGhostbody->getCollisionShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
	}

	m_ptrToPhysicWorld->updateSingleAabb(m_bulletGhostbody);

	m_translation = translation;
	m_rotation = rotation;
	m_scale = scale;
}

void CharacterController::applyTransformFromPhysicSimulation(const glm::vec3 & translation, const glm::quat & rotation)
{
	m_translation = translation;
	m_rotation = rotation;
}

void CharacterController::checkBlockingByCollision(glm::vec3& nextPosition)
{
	btTransform nextTransform_x;
	nextTransform_x.setRotation(btQuaternion(m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w));
	nextTransform_x.setOrigin(btVector3(nextPosition.x, m_translation.y, m_translation.z));

	btTransform nextTransform_y;
	nextTransform_y.setRotation(btQuaternion(m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w));
	nextTransform_y.setOrigin(btVector3(m_translation.x, nextPosition.y, m_translation.z));

	btTransform nextTransform_z;
	nextTransform_z.setRotation(btQuaternion(m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w));
	nextTransform_z.setOrigin(btVector3(m_translation.x, m_translation.y, nextPosition.z));

	btCollisionWorld::ClosestConvexResultCallback cb_x(btVector3(m_translation.x, m_translation.y, m_translation.z), btVector3(nextPosition.x, m_translation.y, m_translation.z));
	m_bulletGhostbody->convexSweepTest(m_shape, m_bulletGhostbody->getWorldTransform(), nextTransform_x, cb_x);
	if (cb_x.hasHit())
		nextPosition.x = m_translation.x;
	
	btCollisionWorld::ClosestConvexResultCallback cb_y(btVector3(m_translation.x, m_translation.y, m_translation.z), btVector3(m_translation.x, nextPosition.y, m_translation.z));
	m_bulletGhostbody->convexSweepTest(m_shape, m_bulletGhostbody->getWorldTransform(), nextTransform_y, cb_y);
	if (cb_y.hasHit()) {
		nextPosition.y = m_translation.y;
		m_isOnGround = true;
	}

	btCollisionWorld::ClosestConvexResultCallback cb_z(btVector3(m_translation.x, m_translation.y, m_translation.z), btVector3(m_translation.x, m_translation.y, nextPosition.z));
	m_bulletGhostbody->convexSweepTest(m_shape, m_bulletGhostbody->getWorldTransform(), nextTransform_z, cb_z);
	if (cb_z.hasHit())
		nextPosition.z = m_translation.z;
}

void CharacterController::update(float deltaTime)
{
	if (m_force.x*m_force.x + m_force.y*m_force.y + m_force.z*m_force.z < 0.001f)
		return;
	glm::vec3 nextPosition;

	m_speed += (deltaTime / 1.f)*m_force;
	m_translation += deltaTime*m_speed;

	checkBlockingByCollision(nextPosition);

	if (m_entity != nullptr)
		entity()->translate(nextPosition);

	m_force = glm::vec3(0, 0, 0);
}

void CharacterController::setGravity(float gravityFactor)
{
	m_gravityFactor = gravityFactor;
}

float CharacterController::getGravity() const
{
	return m_gravityFactor;
}

void CharacterController::setSpeed(Direction direction, float speedFactor)
{
	m_speedFactor[direction] = speedFactor;
}

float CharacterController::getSpeed(Direction direction) const
{
	return m_speedFactor[direction];
}

void CharacterController::setSpeed(float speedFactor)
{
	for (int i = 0; i < 4; i++) {
		m_speedFactor[i] = speedFactor;
	}
}

void CharacterController::setJumpFactor(float jumpFactor)
{
	m_jumpFactor = jumpFactor;
}

float CharacterController::getJumpFactor() const
{
	return m_jumpFactor;
}

void CharacterController::setHeight(float height)
{
	m_height = height;
	makeShape();
}

float CharacterController::getHeight() const
{
	return m_height;
}

void CharacterController::setRadius(float radius)
{
	m_radius = radius;
	makeShape();
}

float CharacterController::getRadius() const
{
	return m_radius;
}

void CharacterController::jump()
{
	if (!m_isOnGround)
		return;

	m_isJumping = true;
	m_force += glm::vec3(0, m_jumpFactor, 0);
}

void CharacterController::move(glm::vec3 direction)
{
	float xSpeed = direction.x * (direction.x > 0 ? m_speedFactor[Direction::RIGHT] : m_speedFactor[Direction::LEFT]);
	float zSpeed = direction.z * (direction.z > 0 ? m_speedFactor[Direction::FORWARD]: m_speedFactor[Direction::BACKWARD]);
	float ySpeed = 1.f;

	m_force += glm::vec3(direction.x * xSpeed, direction.y * ySpeed, direction.z * zSpeed);
}

void CharacterController::drawUI(Scene & scene)
{
	ImGui::InputFloat("forward speed", &m_speedFactor[Direction::FORWARD]);
	ImGui::InputFloat("backward speed", &m_speedFactor[Direction::BACKWARD]);
	ImGui::InputFloat("right speed", &m_speedFactor[Direction::RIGHT]);
	ImGui::InputFloat("left speed", &m_speedFactor[Direction::LEFT]);

	ImGui::InputFloat("gravity", &m_gravityFactor);
	ImGui::InputFloat("jump", &m_jumpFactor);

	float tmpFloat = m_height;
	if (ImGui::InputFloat("height", &tmpFloat)) {
		setHeight(tmpFloat);
	}
	tmpFloat = m_radius;
	if (ImGui::InputFloat("radius", &tmpFloat)) {
		setRadius(tmpFloat);
	}
}

void CharacterController::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

Component* CharacterController::clone(Entity* entity)
{
	CharacterController* newCharacterController = new CharacterController(*this);

	newCharacterController->attachToEntity(entity);

	return newCharacterController;
}


void CharacterController::addToScene(Scene& scene)
{
	scene.add(this);
}

void CharacterController::addToEntity(Entity & entity)
{
	entity.add(this);
}

void CharacterController::eraseFromEntity(Entity& entity)
{
	entity.erase(this);
}

void CharacterController::save(Json::Value & componentRoot) const
{
	componentRoot["translation"] = toJsonValue(m_translation);
	componentRoot["rotation"] = toJsonValue(m_rotation);
	componentRoot["scale"] = toJsonValue(m_scale);
	componentRoot["height"] = m_height;
	componentRoot["radius"] = m_radius;
	componentRoot["jumpFactor"] = m_jumpFactor;
	componentRoot["gravityFactor"] = m_gravityFactor;
	componentRoot["isJumping"] = m_isJumping;
	componentRoot["isOnGround"] = m_isOnGround;

	componentRoot["speed"] = toJsonValue(m_speed);
	componentRoot["force"] = toJsonValue(m_force);

	for (int i = 0; i < 4; i++) {
		componentRoot["speedFactor"][i] = m_speedFactor[i];
	}
}

void CharacterController::load(Json::Value & componentRoot)
{
	m_translation = fromJsonValue<glm::vec3>(componentRoot["translation"], glm::vec3(0, 0, 0));
	m_rotation = fromJsonValue<glm::quat>(componentRoot["rotation"], glm::quat());
	m_scale = fromJsonValue<glm::vec3>(componentRoot["scale"], glm::vec3(0, 0, 0));
	m_height = componentRoot.get("height", 2.f).asFloat();
	m_radius = componentRoot.get("radius", 0.5f).asFloat();
	m_jumpFactor = componentRoot.get("jumpFactor", 1.f).asFloat();
	m_gravityFactor = componentRoot.get("gravityFactor", 1.f).asFloat();
	m_isJumping = componentRoot.get("isJumping", false).asBool();
	m_isOnGround = componentRoot.get("isOnGround", true).asBool();

	m_speed = fromJsonValue<glm::vec3>(componentRoot["speed"], glm::vec3(0, 0, 0));
	m_force = fromJsonValue<glm::vec3>(componentRoot["force"], glm::vec3(0, 0, 0));

	for (int i = 0; i < 4; i++) {
		m_speedFactor[i] = componentRoot["speedFactor"][i].asFloat();
	}

	m_isGhostInWorld = false;
	m_bulletGhostbody = nullptr;
	m_shape = nullptr;
	m_ptrToPhysicWorld = nullptr;
}
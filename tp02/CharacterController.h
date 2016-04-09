#pragma once

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "Component.h"

class CharacterController: public Component
{
	enum Direction {FORWARD = 0, RIGHT = 1, LEFT = 2, BACKWARD = 3};
private:
	glm::vec3 m_translation;
	glm::quat m_rotation;
	glm::vec3 m_scale;

	glm::vec3 m_speed;
	glm::vec3 m_force;

	float m_height;
	float m_radius;

	float m_gravityFactor;
	float m_jumpFactor;
	float m_speedFactor[4]; //one speed per direction

	bool m_isOnGround;
	bool m_isJumping;

	btGhostObject* m_bulletGhostbody;
	bool m_isGhostInWorld;
	btCapsuleShape* m_shape;
	btDiscreteDynamicsWorld* m_ptrToPhysicWorld;

public:
	CharacterController();
	CharacterController(const CharacterController& other);
	CharacterController& operator=(const CharacterController& other);
	~CharacterController();

	void pushToSimulation();
	void popFromSimulation();
	void makeShape();
	void init(btDiscreteDynamicsWorld* physicSimulation);
	void setPtrToPhysicWorld(btDiscreteDynamicsWorld* ptrToPhysicWorld);
	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1, 1, 1), const glm::quat& rotation = glm::quat());
	virtual void applyTransformFromPhysicSimulation(const glm::vec3& translation, const glm::quat& rotation = glm::quat());
	void checkBlockingByCollision(glm::vec3& nextPosition);
	void update(float deltaTime);

	void setGravity(float gravityFactor);
	float getGravity() const;
	void setSpeed(Direction direction, float speedFactor);
	float getSpeed(Direction direction) const;
	void setSpeed(float speedFactor);
	void setJumpFactor(float jumpFactor);
	float getJumpFactor() const;
	void setHeight(float height);
	float getHeight() const;
	void setRadius(float radius);
	float getRadius() const;

	void jump();
	void move(glm::vec3 direction);

	virtual void drawUI(Scene & scene) override;
	virtual void eraseFromScene(Scene & scene) override;
	virtual void addToScene(Scene & scene) override;
	virtual void eraseFromEntity(Entity & entity) override;
	virtual void addToEntity(Entity & entity) override;
	virtual Component * clone(Entity * entity) override;

	virtual void save(Json::Value& componentRoot) const override;
	virtual void load(Json::Value& componentRoot) override;
};


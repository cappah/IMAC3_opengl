#pragma once

#include <iostream>

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "MeshRenderer.h"

#include "Component.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

//forward
class Ray;
class Entity;
class Rigidbody;


struct CollisionInfo {
	Rigidbody* receiver;
	Rigidbody* rigidbody;
	glm::vec3 point;
	glm::vec3 normal;

	CollisionInfo(Rigidbody* _rigidbody = nullptr, const glm::vec3& _point = glm::vec3(0, 0, 0), const glm::vec3& _normal = glm::vec3(0, 0, 0));
};

struct Collider : public Component
{
	MaterialUnlit* visualMaterial;
	Mesh* visualMesh;

	//the offsetPosition is an offset added to the position of the collider
	glm::vec3 offsetPosition;
	//the offsetScale is an offset added to the scale of the collider
	glm::vec3 offsetScale;
	glm::mat4 offsetMatrix;
	//the origin is between 0 and 1, it represent the pivot point of this collider
	glm::vec3 origin;
	glm::vec3 translation;
	glm::vec3 scale;
	glm::quat rotation;
	glm::mat4 modelMatrix;

	Collider(ComponentType colliderType, Mesh* _visualMesh = nullptr, MaterialUnlit* _visualMaterial = nullptr);
	virtual ~Collider();

	//add a visual representation to this collider
	void setVisual(Mesh* _visualMesh, MaterialUnlit* _visualMaterial);

	//apply transform operation to this collider
	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale, const glm::quat& rotation) override;
	virtual void applyTransformFromPhysicSimulation(const glm::vec3& translation, const glm::quat& rotation) override;

	//apply transform operation to this collider
	void applyTransform(const glm::vec3& _translation, const glm::vec3& _scale);

	//keep the translation and rotation, apply only the scale
	void applyScale(const glm::vec3& _scale);

	//keep the translation and rotation, add a scale 
	void appendScale(const glm::vec3& _scale);

	//keep the scale and rotation, apply only the translation
	void applyTranslation(const glm::vec3& _translation);

	//keep the scale and rotation, add a translation 
	void appendTranslation(const glm::vec3& _translation);

	//keep the scale and translation, apply only the rotation 
	void applyRotation(const glm::quat& _rotation);

	//keep the scale and translation, add a rotation 
	void appendRotation(const glm::quat& _rotation);

	//set offset position
	void setOffsetPosition(glm::vec3 _offset);
	//set offset scale
	void setOffsetScale(glm::vec3 _offset);
	//add a delta position to the position offset 
	void addOffsetPosition(glm::vec3 _offset);
	//add a delta scale the the scale offset
	void addOffsetScale(glm::vec3 _offset);

	//set the origin (pivot point between 0 and 1) of the collider
	void setOrigin(const glm::vec3& _origin);

	//get the origin (pivot point between 0 and 1) of the collider
	glm::vec3 getOrigin() const;

	//get the collider modelMatrix
	glm::mat4 getModelMatrix();

	//get the matrix created from the offset position and offset scale
	glm::mat4 getOffsetMatrix() const;

	//update the collider modelMatrix, it is otomatically called by functions that applied transformation on this collider. 
	//You have to override this function to set the proper behaviour of this collider when a transformation is applied to it.
	virtual void updateModelMatrix();

	//update the matrix created from offset scale and offset translation.
	virtual void updateOffsetMatrix();

	//render this collider, if it has a visual representation. With the given color.
	virtual void render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& color = glm::vec3(1, 0, 0)) = 0;

	//print collider info in the console.
	virtual void debugLog() = 0;

	//return true if the collider is intersected by the ray. t is the distance from the origin of the ray, to the intersection.
	virtual bool isIntersectedByRay(const Ray& ray, float* t = nullptr) = 0;

	virtual btCollisionShape* makeShape() = 0;

	virtual void drawUI(Scene& scene) override;

	virtual void eraseFromScene(Scene& scene) override;

	//Cover a mesh, based on the mesh bottomLeft and topRight points and the mesh origin
	virtual void coverMesh(Mesh& mesh) = 0;
	//Cover something which have a bottomLeft/min and topRight/max points and an origin
	virtual void cover(glm::vec3 min, glm::vec3 max, glm::vec3 origin) = 0;

	virtual void save(Json::Value& rootComponent) const override;
	virtual void load(Json::Value& rootComponent) override;

};

struct BoxCollider : public Collider
{
	glm::vec3 localTopRight;
	glm::vec3 localBottomLeft;

	glm::vec3 topRight;
	glm::vec3 bottomLeft;

	BoxCollider(Mesh* _visualMesh = nullptr, MaterialUnlit* _visualMaterial = nullptr);

	virtual void updateModelMatrix() override;
	virtual void render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& color = glm::vec3(1,0,0)) override;
	virtual void debugLog() override;
	virtual bool isIntersectedByRay(const Ray& ray, float* t = nullptr) override;
	virtual void drawUI(Scene& scene) override;
	virtual Component* clone(Entity* entity) override;
	virtual void addToScene(Scene& scene) override;
	virtual void addToEntity(Entity& entity) override;
	virtual void eraseFromEntity(Entity& entity) override;
	virtual void coverMesh(Mesh& mesh) override;
	virtual void cover(glm::vec3 min, glm::vec3 max, glm::vec3 origin) override;
	virtual btCollisionShape* makeShape() override;

	virtual void save(Json::Value& rootComponent) const override;
	virtual void load(Json::Value& rootComponent) override;
};


struct CapsuleCollider : public Collider
{
	float height;
	float radius;

	CapsuleCollider();

	virtual void render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& color = glm::vec3(1, 0, 0)) override;
	virtual void debugLog() override;
	virtual bool isIntersectedByRay(const Ray& ray, float* t = nullptr) override;
	virtual void drawUI(Scene& scene) override;
	virtual Component* clone(Entity* entity) override;
	virtual void addToScene(Scene& scene) override;
	virtual void addToEntity(Entity& entity) override;
	virtual void eraseFromEntity(Entity& entity) override;
	virtual void coverMesh(Mesh& mesh) override;
	virtual void cover(glm::vec3 min, glm::vec3 max, glm::vec3 origin) override;
	virtual btCollisionShape* makeShape() override;

	virtual void save(Json::Value& rootComponent) const override;
	virtual void load(Json::Value& rootComponent) override;
};
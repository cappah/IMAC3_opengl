#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr
#include "glm/gtc/quaternion.hpp"

#include "Utils.h"

#include "Component.h"

#include "Collider.h"

//forward
class Entity;

struct Light : public Component
{
	float intensity;
	glm::vec3 color;


	Light(float _intensity, glm::vec3 _color);
	virtual ~Light();

	float getIntensity() const;
	glm::vec3 getColor() const;
	void setIntensity(float i);
	void setColor(const glm::vec3& c);

	virtual void updateBoundingBox();

	virtual void save(Json::Value& rootComponent) const override;
	virtual void load(Json::Value& rootComponent) override;

};

struct PointLight : public Light
{
	glm::vec3 position;
	BoxCollider boundingBox;

	PointLight(float _intensity = 10, glm::vec3 _color = glm::vec3(1, 1, 1), glm::vec3 _position = glm::vec3(0, 0, 0));
	virtual ~PointLight();

	virtual void updateBoundingBox() override;

	virtual void drawUI(Scene& scene) override;
	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1, 1, 1), const glm::quat& rotation  = glm::quat()) override;
	virtual void eraseFromScene(Scene& scene) override;
	virtual Component* clone(Entity* entity) override;
	virtual void addToScene(Scene& scene) override;
	virtual void addToEntity(Entity& entity) override;
	virtual void eraseFromEntity(Entity& entity) override;

	void setBoundingBoxVisual(Mesh* visualMesh, MaterialUnlit* visualMaterial);
	void renderBoundingBox(const glm::mat4& projectile, const glm::mat4& view, glm::vec3 color);

	virtual void save(Json::Value& rootComponent) const override;
	virtual void load(Json::Value& rootComponent) override;

};

struct DirectionalLight : public Light
{
	glm::vec3 up;

	glm::vec3 direction;

	DirectionalLight(float _intensity = 0.2f, glm::vec3 _color = glm::vec3(1, 1, 1), glm::vec3 _direction = glm::vec3(0, -1, 0));
	virtual ~DirectionalLight();

	virtual void drawUI(Scene& scene) override;
	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1, 1, 1), const glm::quat& rotation = glm::quat()) override;
	virtual void eraseFromScene(Scene& scene) override;
	virtual Component* clone(Entity* entity) override;
	virtual void addToScene(Scene& scene) override;
	virtual void addToEntity(Entity& entity) override;
	virtual void eraseFromEntity(Entity& entity) override;

	virtual void save(Json::Value& rootComponent) const override;
	virtual void load(Json::Value& rootComponent) override;
};

struct SpotLight : public Light
{
	glm::vec3 up;

	glm::vec3 position;
	glm::vec3 direction;
	float angle;

	BoxCollider boundingBox;

	SpotLight(float _intensity = 10, glm::vec3 _color = glm::vec3(1, 1, 1), glm::vec3 _position = glm::vec3(0, 0, 0), glm::vec3 _direction = glm::vec3(0, -1, 0), float _angle = glm::radians(30.f));
	virtual ~SpotLight();

	virtual void updateBoundingBox() override;

	virtual void drawUI(Scene& scene) override;
	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1, 1, 1), const glm::quat& rotation = glm::quat()) override;
	virtual void eraseFromScene(Scene& scene) override;
	virtual Component* clone(Entity* entity) override;
	virtual void addToScene(Scene& scene) override;
	virtual void addToEntity(Entity& entity) override;
	virtual void eraseFromEntity(Entity& entity) override;

	void setBoundingBoxVisual(Mesh* visualMesh, MaterialUnlit* visualMaterial);
	void renderBoundingBox(const glm::mat4& projectile, const glm::mat4& view, glm::vec3 color);

	virtual void save(Json::Value& rootComponent) const override;
	virtual void load(Json::Value& rootComponent) override;
};


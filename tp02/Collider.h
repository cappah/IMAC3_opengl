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

//forward
class Ray;
class Entity;

struct Collider : public Component
{
	MeshRenderer* visual;

	glm::vec3 offsetPosition;
	glm::vec3 offsetScale;

	glm::vec3 translation;
	glm::vec3 scale;
	glm::mat4 modelMatrix;

	Collider(MeshRenderer* _visual);
	virtual ~Collider();

	void setVisual(MeshRenderer* _visual);

	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale, const glm::quat& rotation) override;

	void applyTransform(const glm::vec3& _translation, const glm::vec3& _scale);

	//keep the translation, apply only the scale
	void applyScale(const glm::vec3& _scale);

	//keep the translation, append the scale 
	void appendScale(const glm::vec3& _scale);

	//keep the scale, apply only the translation
	void applyTranslation(const glm::vec3& _translation);

	//keep the scale, append the translation 
	void appendTranslation(const glm::vec3& _translation);

	void setOffsetPosition(glm::vec3 _offset);

	void setOffsetScale(glm::vec3 _offset);

	glm::mat4 getModelMatrix();

	virtual void updateModelMatrix() = 0;

	virtual void render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& color = glm::vec3(1, 0, 0)) = 0;

	virtual void debugLog() = 0;

	virtual bool isIntersectedByRay(const Ray& ray, float* t = nullptr) = 0;

	virtual void drawUI() override;

	virtual void eraseFromScene(Scene& scene) override;

};

struct BoxCollider : public Collider
{
	glm::vec3 localTopRight;
	glm::vec3 localBottomLeft;

	glm::vec3 topRight;
	glm::vec3 bottomLeft;

	BoxCollider(MeshRenderer* _visual = nullptr);

	virtual void updateModelMatrix() override;
	virtual void render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& color = glm::vec3(1,0,0)) override;
	virtual void debugLog() override;
	virtual bool isIntersectedByRay(const Ray& ray, float* t = nullptr) override;
	virtual void drawUI() override;
	virtual Component* clone(Entity* entity) override;
	virtual void addToScene(Scene& scene) override;
};
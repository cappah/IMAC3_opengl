#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr


class TransformNode
{
protected:
	glm::vec3 m_translation;
	glm::vec3 m_scale;
	glm::quat m_rotation;
	glm::vec3 m_eulerRotation;

	glm::vec3 m_localTranslation;
	glm::vec3 m_localScale;
	glm::quat m_localRotation;
	glm::vec3 m_localEulerRotation;

	glm::mat4 m_modelMatrix;
public:
	TransformNode();
	virtual ~TransformNode();

	glm::mat4 getModelMatrix();

	glm::vec3 getTranslation();
	glm::vec3 getScale();
	glm::quat getRotation();
	glm::vec3 getEulerRotation();

	void translate(glm::vec3 const& t);
	void setTranslation(glm::vec3 const& t);
	void scale(glm::vec3 const& s);
	void setScale(glm::vec3 const& s);
	void rotate(glm::quat const& q);
	void setRotation(glm::quat const& q);
	void setEulerRotation(glm::vec3 const& q);

	void updateModelMatrix();

	virtual void onChangeModelMatrix() = 0;

	//apply transform on this entity, and apply transform on all its components.
	virtual void applyTransform() = 0;
	//function to apply transform to all children.
	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1, 1, 1), const glm::quat& rotation = glm::quat()) = 0;

	void drawUI();
};


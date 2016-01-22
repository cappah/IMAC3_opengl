#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "MeshRenderer.h"

struct Collider
{
	glm::vec3 localTopRight;
	glm::vec3 localBottomLeft;

	glm::vec3 topRight;
	glm::vec3 bottomLeft;

	MeshRenderer* visual;

	Collider(MeshRenderer* _visual = nullptr);

	void setVisual(MeshRenderer* _visual);

	void applyTransform(const glm::vec3& translation, const glm::vec3& scale);

	//keep the translation, apply only the scale
	void applyScale(const glm::vec3& scale);

	//keep the translation, append the scale 
	void appendScale(const glm::vec3& scale);

	//keep the scale, apply only the translation
	void applyTranslation(const glm::vec3& translation);

	//keep the scale, append the translation 
	void appendTranslation(const glm::vec3& translation);

};
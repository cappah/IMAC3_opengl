#pragma once

#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "MeshRenderer.h"
#include "Collider.h"

struct Entity
{
	std::string name;

	glm::mat4 modelMatrix;
	MeshRenderer* meshRenderer;
	Collider* collider;

	Entity();

	void applyTransform();

	void drawUI();
};

#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "Utils.h"

struct Light
{
	float intensity;
	glm::vec3 color;

	Light(float _intensity, glm::vec3 _color);
};

struct PointLight : public Light
{
	glm::vec3 position;

	PointLight(float _intensity, glm::vec3 _color, glm::vec3 _position);

	void drawUI(int id);
};

struct DirectionalLight : public Light
{
	glm::vec3 direction;

	DirectionalLight(float _intensity, glm::vec3 _color, glm::vec3 _direction);

	void drawUI(int id);
};

struct SpotLight : public Light
{
	glm::vec3 position;
	glm::vec3 direction;
	float angle;

	SpotLight(float _intensity, glm::vec3 _color, glm::vec3 _position, glm::vec3 _direction, float _angle);

	void drawUI(int id);
};


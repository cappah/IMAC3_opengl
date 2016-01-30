#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

struct Camera
{
	float radius;
	float theta;
	float phi;
	glm::vec3 o;
	glm::vec3 eye;
	glm::vec3 up;
};
void camera_defaults(Camera & c);
void camera_zoom(Camera & c, float factor);
void camera_turn(Camera & c, float phi, float theta);
void camera_pan(Camera & c, float x, float y);
void camera_rotate(Camera& c, float phi, float theta);
void camera_translate(Camera& c, float x, float y, float z);
void toogleCamera(Camera& c);

#pragma once

#include <iostream>

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "Collider.h"

class Ray
{
private:
	glm::vec3 direction;
	glm::vec3 origin;
	float length;

public:
	Ray(glm::vec3 _origin, glm::vec3 _direction, float _length = 100.f);

	glm::vec3 at(float t);

	bool intersect(Collider& other);

	void debugLog();
};

#include "Ray.h"

Ray::Ray(glm::vec3 _origin, glm::vec3 _direction, float _length) : origin(_origin), direction(_direction), length(_length)
{

}

glm::vec3 Ray::at(float t) const
{
	return origin + t*direction;
}

glm::vec3 Ray::getDirection() const
{
	return direction;
}

glm::vec3 Ray::getOrigin() const
{
	return origin;
}

float Ray::getLength() const
{
	return length;
}

bool Ray::intersect(Collider& other, float* t)
{
	return other.isIntersectedByRay(*this, t);
}

bool Ray::intersectPlane(const glm::vec3& anchor, const glm::vec3& normal, float* t) const
{
	float normalDotDirection = glm::dot(normal, direction);
	if (normalDotDirection == 0)
		return false;

	if (t != nullptr)
		*t = glm::dot((anchor - origin), normal) / normalDotDirection;
	return true;
}

void Ray::debugLog()
{
	std::cout << "ray origin : (" << origin.x <<", "<<origin.y<<", "<<origin.z << "), ray direction : (" << direction.x << ", " << direction.y << ", " << direction.z << "), ray lenght : " << length << std::endl;
}
#include "Ray.h"

Ray::Ray(glm::vec3 _origin, glm::vec3 _direction, float _length) : origin(_origin), direction(_direction), length(_length)
{

}

glm::vec3 Ray::at(float t)
{
	return origin + t*direction;
}

bool Ray::intersect(Collider& other)
{
	glm::vec3 t;
	int maxIndex = 0;
	for (int i = 0; i < 3; i++)
	{
		if (direction.x > 0)
			t[i] = (other.bottomLeft[i] - origin[i]) / direction[i];
		else
			t[i] = (other.topRight[i] - origin[i]) / direction[i];

		if (t[i] > t[maxIndex])
		{
			maxIndex = i;
		}
	}

	if (t[maxIndex] >= 0 && t[maxIndex] < length)
	{
		glm::vec3 pt = at(t[maxIndex]);

		int o1 = (maxIndex + 1) % 3;
		int o2 = (maxIndex + 2) % 3;

		return((pt[o1] > other.bottomLeft[o1] && pt[o1] < other.topRight[o1]) &&
			(pt[o2] > other.bottomLeft[o2] && pt[o2] < other.topRight[o2]));
	}
}
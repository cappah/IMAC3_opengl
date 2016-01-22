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
	// r.dir is unit direction vector of ray
	glm::vec3 dirfrac;
	dirfrac.x = 1.0f / direction.x;
	dirfrac.y = 1.0f / direction.y;
	dirfrac.z = 1.0f / direction.z;
	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray
	float t1 = (other.bottomLeft.x- origin.x)*dirfrac.x;
	float t2 = (other.topRight.x - origin.x)*dirfrac.x;
	float t3 = (other.bottomLeft.y - origin.y)*dirfrac.y;
	float t4 = (other.topRight.y - origin.y)*dirfrac.y;
	float t5 = (other.bottomLeft.z - origin.z)*dirfrac.z;
	float t6 = (other.topRight.z - origin.z)*dirfrac.z;

	float tmin = glm::max<float>(glm::max<float>(glm::min<float>(t1, t2), glm::min<float>(t3, t4)), glm::min<float>(t5, t6));
	float tmax = glm::min<float>(glm::min<float>(glm::max<float>(t1, t2), glm::max<float>(t3, t4)), glm::max<float>(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0)
	{
		//t = tmax;
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		//t = tmax;
		return false;
	}

	//t = tmin;
	return true;

	/*


	glm::vec3 t;
	int maxIndex = 0;
	for (int i = 0; i < 3; i++)
	{
		if (direction[i] > 0)
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

	return false;*/
}

void Ray::debugLog()
{
	std::cout << "ray origin : (" << origin.x <<", "<<origin.y<<", "<<origin.z << "), ray direction : (" << direction.x << ", " << direction.y << ", " << direction.z << "), ray lenght : " << length << std::endl;
}
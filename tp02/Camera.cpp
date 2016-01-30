
#include "Camera.h"

void camera_compute(Camera & c)
{
	c.eye.x = cos(c.theta) * sin(c.phi) * c.radius + c.o.x;
	c.eye.y = cos(c.phi) * c.radius + c.o.y;
	c.eye.z = sin(c.theta) * sin(c.phi) * c.radius + c.o.z;
	c.up = glm::vec3(0.f, c.phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
}

void camera_compute_fps(Camera & c)
{
	c.o.x = cos(c.theta) * sin(c.phi) * c.radius + c.eye.x;
	c.o.y = cos(c.phi) * c.radius + c.eye.y;
	c.o.z = sin(c.theta) * sin(c.phi) * c.radius + c.eye.z;
	c.up = glm::vec3(0.f, c.phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
}

void camera_defaults(Camera & c)
{
	c.phi = 3.14 / 2.f;
	c.theta = 3.14 / 2.f;
	c.radius = 10.f;
	camera_compute(c);
}

void camera_zoom(Camera & c, float factor)
{
	c.radius += factor * c.radius;
	if (c.radius < 0.1)
	{
		c.radius = 10.f;
		c.o = c.eye + glm::normalize(c.o - c.eye) * c.radius;
	}
	camera_compute(c);
}

void camera_turn(Camera & c, float phi, float theta)
{
	c.theta += 1.f * theta;
	c.phi -= 1.f * phi;
	if (c.phi >= (2 * glm::pi<float>()) - 0.1)
		c.phi = 0.00001;
	else if (c.phi <= 0)
		c.phi = 2 * glm::pi<float>() - 0.1;
	camera_compute(c);
}

void camera_pan(Camera & c, float x, float y)
{
	glm::vec3 up(0.f, c.phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	glm::vec3 fwd = glm::normalize(c.o - c.eye);
	glm::vec3 side = glm::normalize(glm::cross(fwd, up));
	c.up = glm::normalize(glm::cross(side, fwd));
	c.o[0] += up[0] * y * c.radius * 2;
	c.o[1] += up[1] * y * c.radius * 2;
	c.o[2] += up[2] * y * c.radius * 2;
	c.o[0] -= side[0] * x * c.radius * 2;
	c.o[1] -= side[1] * x * c.radius * 2;
	c.o[2] -= side[2] * x * c.radius * 2;
	camera_compute(c);
}

void camera_rotate(Camera & c, float phi, float theta)
{
	c.theta += 1.f * theta;
	c.phi += 1.f * phi;
	if (c.phi >= (2 * glm::pi<float>()) - 0.1)
		c.phi = 0.00001;
	else if (c.phi <= 0)
		c.phi = 2 * glm::pi<float>() - 0.1;

	camera_compute_fps(c);
}

void camera_translate(Camera & c, float x, float y, float z)
{
	glm::vec3 up(0.f, c.phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	glm::vec3 fwd = glm::normalize(c.o - c.eye);
	glm::vec3 side = glm::normalize(glm::cross(fwd, up));
	c.up = glm::normalize(glm::cross(side, fwd));

	c.eye[0] += up[0] * y * c.radius * 2;
	c.eye[1] += up[1] * y * c.radius * 2;
	c.eye[2] += up[2] * y * c.radius * 2;

	c.eye[0] += side[0] * x * c.radius * 2;
	c.eye[1] += side[1] * x * c.radius * 2;
	c.eye[2] += side[2] * x * c.radius * 2;

	c.eye[0] += fwd[0] * z * c.radius * 2;
	c.eye[1] += fwd[1] * z * c.radius * 2;
	c.eye[2] += fwd[2] * z * c.radius * 2;

	camera_compute_fps(c);
}

void toogleCamera(Camera& c)
{
	c.theta += glm::pi<float>();
	c.phi = glm::pi<float>() - c.phi ;
}

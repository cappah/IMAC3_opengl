
#include "Camera.h"


Camera::Camera() : 
	radius(1), theta(glm::pi<float>()*0.5f), phi(0), o(0), eye(o), up(glm::vec3(0,1,0)), dir(1,0,0)
{
	
}

Camera::Camera(const Camera & other) :
	radius(other.radius), theta(other.theta), phi(other.phi), o(other.o), eye(other.eye), up(other.up), dir(other.dir)
{

}

///////////////////////////

void CameraFPS::setTranslation(glm::vec3 pos)
{
	eye[0] = pos.x;
	eye[1] = pos.y;
	eye[2] = pos.z;

	updateTransform();
}

void CameraFPS::translate(glm::vec3 pos)
{
	eye[0] += pos.x;
	eye[1] += pos.y;
	eye[2] += pos.z;

	updateTransform();
}


CameraFPS::CameraFPS() : Camera()
{
	updateTransform();
}

void CameraFPS::setTranslationLocal(glm::vec3 pos)
{
	glm::vec3 up(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	glm::vec3 fwd = glm::normalize(o - eye);
	glm::vec3 side = glm::normalize(glm::cross(fwd, up));
	up = glm::normalize(glm::cross(side, fwd));

	eye[0] = up[0] * pos.y;
	eye[1] = up[1] * pos.y;
	eye[2] = up[2] * pos.y;

	eye[0] = side[0] * pos.x;
	eye[1] = side[1] * pos.x;
	eye[2] = side[2] * pos.x;

	eye[0] = fwd[0] * pos.z;
	eye[1] = fwd[1] * pos.z;
	eye[2] = fwd[2] * pos.z;

	updateTransform();
}

void CameraFPS::translateLocal(glm::vec3 pos)
{
	glm::vec3 up(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	glm::vec3 fwd = glm::normalize(o - eye);
	glm::vec3 side = glm::normalize(glm::cross(fwd, up));
	up = glm::normalize(glm::cross(side, fwd));

	eye[0] += up[0] * pos.y;
	eye[1] += up[1] * pos.y;
	eye[2] += up[2] * pos.y;

	eye[0] += side[0] * pos.x;
	eye[1] += side[1] * pos.x;
	eye[2] += side[2] * pos.x;

	eye[0] += fwd[0] * pos.z;
	eye[1] += fwd[1] * pos.z;
	eye[2] += fwd[2] * pos.z;

	updateTransform();
}

void CameraFPS::setDirection(glm::vec3 dir)
{
	float r = std::sqrt(dir.x*dir.x + dir.z*dir.z);
	phi = dir.y == glm::pi<float>()*0.5f ? 0 : std::atanf(r / dir.y);
	theta = dir.x == glm::pi<float>()*0.5f ? 0 : std::atanf(dir.z / dir.x);

	if (phi >= (2 * glm::pi<float>()) - 0.1)
		phi = 0.00001;
	else if (phi <= 0)
		phi = 2 * glm::pi<float>() - 0.1;

	o = dir;

	up = glm::vec3(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
}

void CameraFPS::setRotation(float phi, float theta)
{
	theta += 1.f * theta;
	phi += 1.f * phi;
	if (phi >= (2 * glm::pi<float>()) - 0.1)
		phi = 0.00001;
	else if (phi <= 0)
		phi = 2 * glm::pi<float>() - 0.1;

	updateTransform();
}

void CameraFPS::updateTransform()
{
	o.x = cos(theta) * sin(phi) + eye.x;
	o.y = cos(phi) + eye.y;
	o.z = sin(theta) * sin(phi) + eye.z;
	up = glm::vec3(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	dir = glm::normalize(o - eye);
}

void CameraFPS::switchFromCameraEditor(const Camera & other)
{
	theta += glm::pi<float>();
	phi = glm::pi<float>() - phi;

	updateTransform();
}

//////////////////////////////////



CameraEditor::CameraEditor() : Camera()
{
	updateTransform();
}

void CameraEditor::setTranslationLocal(glm::vec3 pos)
{
	glm::vec3 up(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	glm::vec3 fwd = glm::normalize(o - eye);
	glm::vec3 side = glm::normalize(glm::cross(fwd, up));
	up = glm::normalize(glm::cross(side, fwd));

	o[0] = up[0] * pos.y * radius * 2;
	o[1] = up[1] * pos.y * radius * 2;
	o[2] = up[2] * pos.y * radius * 2;
	o[0] = -side[0] * pos.x * radius * 2;
	o[1] = -side[1] * pos.x * radius * 2;
	o[2] = -side[2] * pos.x * radius * 2;

	updateTransform();
}

void CameraEditor::translateLocal(glm::vec3 pos)
{
	glm::vec3 up(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	glm::vec3 fwd = glm::normalize(o - eye);
	glm::vec3 side = glm::normalize(glm::cross(fwd, up));
	up = glm::normalize(glm::cross(side, fwd));

	o[0] += up[0] * pos.y * radius * 2;
	o[1] += up[1] * pos.y * radius * 2;
	o[2] += up[2] * pos.y * radius * 2;
	o[0] -= side[0] * pos.x * radius * 2;
	o[1] -= side[1] * pos.x * radius * 2;
	o[2] -= side[2] * pos.x * radius * 2;

	updateTransform();
}

void CameraEditor::setDirection(glm::vec3 dir)
{
	float r = std::sqrt(dir.x*dir.x + dir.z*dir.z);
	phi = dir.y == glm::pi<float>()*0.5f ? 0 : std::atanf(r / dir.y);
	theta = dir.x == glm::pi<float>()*0.5f ? 0 : std::atanf(dir.z / dir.x);

	if (phi >= (2 * glm::pi<float>()) - 0.1)
		phi = 0.00001;
	else if (phi <= 0)
		phi = 2 * glm::pi<float>() - 0.1;

	updateTransform();
}

void CameraEditor::setRotation(float phi, float theta)
{
	theta += 1.f * theta;
	phi -= 1.f * phi;

	if (phi >= (2 * glm::pi<float>()) - 0.1)
		phi = 0.00001;
	else if (phi <= 0)
		phi = 2 * glm::pi<float>() - 0.1;

	updateTransform();
}

void CameraEditor::setTranslation(glm::vec3 pos)
{
	o[0] = pos.y;
	o[1] = 0;
	o[2] = 0;

	o[0] = 0;
	o[1] = pos.x;
	o[2] = 0;

	o[0] = 0;
	o[1] = 0;
	o[2] = pos.z;

	updateTransform();
}

void CameraEditor::translate(glm::vec3 pos)
{
	o[0] += pos.y;
	o[1] += 0;
	o[2] += 0;

	o[0] += 0;
	o[1] += pos.x;
	o[2] += 0;

	o[0] += 0;
	o[1] += 0;
	o[2] += pos.z;

	updateTransform();
}

void CameraEditor::updateTransform()
{
	eye.x = cos(theta) * sin(phi) * radius + o.x;
	eye.y = cos(phi) * radius + o.y;
	eye.z = sin(theta) * sin(phi) * radius + o.z;
	up = glm::vec3(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);

	dir = glm::normalize(o - eye);
}

void CameraEditor::switchFromCameraFPS(const Camera & other)
{
	theta += glm::pi<float>();
	phi = glm::pi<float>() - phi;

	updateTransform();
}


//////////////////////////////////

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

	c.eye[0] += up[0] * y;
	c.eye[1] += up[1] * y;
	c.eye[2] += up[2] * y;

	c.eye[0] += side[0] * x;
	c.eye[1] += side[1] * x;
	c.eye[2] += side[2] * x;

	c.eye[0] += fwd[0] * z;
	c.eye[1] += fwd[1] * z;
	c.eye[2] += fwd[2] * z;

	camera_compute_fps(c);
}

void toogleCamera(Camera& c)
{
	c.theta += glm::pi<float>();
	c.phi = glm::pi<float>() - c.phi;
}
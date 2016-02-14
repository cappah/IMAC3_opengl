#include "Camera.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

Camera::Camera() : Component(ComponentType::CAMERA),
	theta(glm::pi<float>()*0.5f), phi(0), o(0), eye(o), up(0,1,0), forward(0,0,1), right(1,0,0)
{
	
}

Camera::Camera(const Camera & other) : Component(ComponentType::CAMERA),
	theta(other.theta), phi(other.phi), o(other.o), eye(other.eye), up(other.up), forward(other.forward), right(other.right)
{

}

///////////////////////////


CameraFPS::CameraFPS() : Camera()
{
	updateTransform();
}

CameraFPS::CameraFPS(const Camera & cam) : Camera(cam)
{
}


void CameraFPS::setTranslation(glm::vec3 pos)
{
	eye = pos;

	updateTransform();
}

void CameraFPS::translate(glm::vec3 pos)
{
	eye += pos;

	updateTransform();
}

void CameraFPS::rotate(float deltaX, float deltaY)
{
	theta += 1.f * deltaX;
	phi += 1.f * deltaY;
	if (phi >= (2 * glm::pi<float>()) - 0.1)
		phi = 0.00001;
	else if (phi <= 0)
		phi = 2 * glm::pi<float>() - 0.1;

	forward.x = cos(theta) * sin(phi);
	forward.y = cos(phi);
	forward.z = sin(theta) * sin(phi);

	updateTransform();
}

void CameraFPS::setTranslationLocal(glm::vec3 pos)
{
	eye = glm::vec3(0, 0, 0);

	eye += up * pos.y;
	eye += right * pos.x;
	eye += forward * pos.z;

	updateTransform();
}

void CameraFPS::translateLocal(glm::vec3 pos)
{
	//glm::vec3 up(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	//glm::vec3 fwd = glm::normalize(o - eye);
	//glm::vec3 side = glm::normalize(glm::cross(fwd, up));
	//up = glm::normalize(glm::cross(side, fwd));

	eye += up * pos.y;
	eye += right * pos.x;
	eye += forward * pos.z;

	updateTransform();
}

void CameraFPS::setDirection(glm::vec3 _dir)
{
	//forward = glm::normalize(_dir);

	//float r = std::sqrt(forward.x*forward.x + forward.z*forward.z);
	//phi = atan2(r, forward.y) + glm::pi<float>();
	//theta = atan2(forward.z, forward.x) + glm::pi<float>();

	//if (phi >= (2 * glm::pi<float>()) - 0.1)
	//	phi = 0.00001;
	//else if (phi <= 0)
	//	phi = 2 * glm::pi<float>() - 0.1;

	//o = eye + forward;

	//up = glm::vec3(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);

	forward = _dir;

	updateTransform();
}

void CameraFPS::setRotation(float _phi, float _theta)
{
	theta = 1.f * theta;
	phi = 1.f * phi;
	if (phi >= (2 * glm::pi<float>()) - 0.1)
		phi = 0.00001;
	else if (phi <= 0)
		phi = 2 * glm::pi<float>() - 0.1;

	forward.x = cos(theta) * sin(phi);
	forward.y = cos(phi);
	forward.z = sin(theta) * sin(phi);

	updateTransform();
}

void CameraFPS::updateTransform()
{
	forward = glm::normalize(forward);

	up = glm::vec3(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	right = glm::normalize(glm::cross(forward, up));
	up = glm::normalize(glm::cross(right, forward));

	o = forward + eye;
}

void CameraFPS::switchFromCameraEditor(const Camera & other)
{
	eye = other.o;

	theta += glm::pi<float>();
	phi = glm::pi<float>() - phi;

	updateTransform();
}

Component * CameraFPS::clone(Entity * entity)
{
	CameraFPS* camera = new CameraFPS(*this);

	camera->attachToEntity(entity);

	return camera;
}

//////////////////////////////////



CameraEditor::CameraEditor() : Camera(), radius(3.f)
{
	updateTransform();
}

CameraEditor::CameraEditor(const Camera & cam) : Camera(cam), radius(3.f)
{
}

void CameraEditor::setTranslationLocal(glm::vec3 pos)
{
	//glm::vec3 up(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	//glm::vec3 fwd = glm::normalize(o - eye);
	//glm::vec3 side = glm::normalize(glm::cross(fwd, up));
	//up = glm::normalize(glm::cross(side, fwd));

	o = up * pos.y * radius * 2.f;
	o = -right * pos.x * radius * 2.f;
	o = forward * pos.z * radius * 2.f;

	updateTransform();
}

void CameraEditor::translateLocal(glm::vec3 pos)
{
	//glm::vec3 up(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	//glm::vec3 fwd = glm::normalize(o - eye);
	//glm::vec3 side = glm::normalize(glm::cross(fwd, up));
	//up = glm::normalize(glm::cross(side, fwd));

	o += up * pos.y * radius * 2.f;
	o += -right * pos.x * radius * 2.f;
	o += forward * pos.z * radius * 2.f;

	updateTransform();
}

void CameraEditor::setDirection(glm::vec3 dir)
{
	//float r = std::sqrt(dir.x*dir.x + dir.z*dir.z);
	//phi = dir.y == glm::pi<float>()*0.5f ? 0 : std::atanf(r / dir.y);
	//theta = dir.x == glm::pi<float>()*0.5f ? 0 : std::atanf(dir.z / dir.x);

	//if (phi >= (2 * glm::pi<float>()) - 0.1)
	//	phi = 0.00001;
	//else if (phi <= 0)
	//	phi = 2 * glm::pi<float>() - 0.1;

	forward = dir;

	updateTransform();
}

void CameraEditor::setRotation(float phi, float theta)
{
	theta = 1.f * theta;
	phi = 1.f * phi;

	if (phi >= (2 * glm::pi<float>()) - 0.1)
		phi = 0.00001;
	else if (phi <= 0)
		phi = 2 * glm::pi<float>() - 0.1;

	forward.x = cos(theta) * sin(phi);
	forward.y = cos(phi);
	forward.z = sin(theta) * sin(phi);

	updateTransform();
}

void CameraEditor::setTranslation(glm::vec3 pos)
{
	o = pos;

	updateTransform();
}

void CameraEditor::translate(glm::vec3 pos)
{
	o += pos;

	updateTransform();
}

void CameraEditor::rotate(float deltaX, float deltaY)
{
	theta += 1.f * deltaX;
	phi += 1.f * deltaY;
	if (phi >= (2 * glm::pi<float>()) - 0.1)
		phi = 0.00001;
	else if (phi <= 0)
		phi = 2 * glm::pi<float>() - 0.1;


	forward.x = cos(theta) * sin(phi);
	forward.y = cos(phi);
	forward.z = sin(theta) * sin(phi);


	updateTransform();
}

void CameraEditor::updateTransform()
{
	forward = glm::normalize(forward);

	up = glm::vec3(0.f, phi < glm::pi<float>() ? 1.f : -1.f, 0.f);
	right = glm::normalize(glm::cross(forward, up));
	up = glm::normalize(glm::cross(right, forward));

	eye = o - forward*radius;
}

void CameraEditor::switchFromCameraFPS(const Camera & other)
{
	eye = other.o;

	theta += glm::pi<float>();
	phi = glm::pi<float>() - phi;

	updateTransform();
}

Component * CameraEditor::clone(Entity * entity)
{
	CameraEditor* camera = new CameraEditor(*this);

	camera->attachToEntity(entity);

	return camera;
}


//////////////////////////////////
/*

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
*/

void Camera::drawUI()
{
	if (ImGui::CollapsingHeader("camera"))
	{
		//TODO
	}
}

void Camera::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

void Camera::addToScene(Scene & scene)
{
	scene.add(this);
}

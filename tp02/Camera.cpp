#include "stdafx.h"

#include "Camera.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

BaseCamera::BaseCamera()
{
}

////////////////////////////


Camera::Camera() : Component(ComponentType::CAMERA),
	m_lookPosition(0,0,0), m_position(0,0,-1), m_up(0,1,0), m_forward(0,0,1),
	m_cameraMode(CameraMode::PERSPECTIVE), m_fovy(45.0f), m_aspect(16.f / 9.f), m_zNear(0.1f), m_zFar(100.f), m_left(-10.f), m_top(10.f), m_right(10.f), m_bottom(-10.f)
	
{
	m_projectionMatrix = glm::perspective(m_fovy, m_aspect, m_zNear, m_zFar);
}

void Camera::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
{

	m_forward = rotation * glm::vec3(0, 0, 1);
	m_forward = glm::normalize(m_forward);

	m_up = rotation * glm::vec3(0, 1, 0);

	m_lookPosition = m_position + m_forward;

	m_viewMatrix = glm::lookAt(m_position, m_lookPosition, m_up);
}

void Camera::drawInInspector(Scene& scene)
{
	if (ImGui::RadioButton("perspective", (m_cameraMode == CameraMode::PERSPECTIVE)))
	{
		m_cameraMode = CameraMode::PERSPECTIVE;
	}
	if(ImGui::RadioButton("orthographic", (m_cameraMode == CameraMode::ORTHOGRAPHIC)))
	{
		m_cameraMode = CameraMode::ORTHOGRAPHIC;
	}
	if (ImGui::SliderFloat("fov", &(m_fovy), 0.f, glm::pi<float>()))
	{
		updateProjection();
	}
	if (ImGui::SliderFloat("near", &(m_zNear), 0.001f, 5.f))
	{
		updateProjection();
	}
	if (ImGui::SliderFloat("far", &(m_zFar), 0.01f, 1000.f))
	{
		updateProjection();
	}
	if (ImGui::SliderFloat("aspect", &(m_aspect), 0.01f, 10.f))
	{
		updateProjection();
	}
}

void Camera::drawInInspector(Scene& scene, const std::vector<Component*>& components)
{
	if (ImGui::RadioButton("perspective", (m_cameraMode == CameraMode::PERSPECTIVE)))
	{
		for (auto component : components)
		{
			Camera* castedComponent = static_cast<Camera*>(component);
			castedComponent->m_cameraMode = CameraMode::PERSPECTIVE;
		}
	}
	if (ImGui::RadioButton("orthographic", (m_cameraMode == CameraMode::ORTHOGRAPHIC)))
	{
		for (auto component : components)
		{
			Camera* castedComponent = static_cast<Camera*>(component);
			castedComponent->m_cameraMode = CameraMode::ORTHOGRAPHIC;
		}
	}
	if (ImGui::SliderFloat("fov", &(m_fovy), 0.f, glm::pi<float>()))
	{
		for (auto component : components)
		{
			Camera* castedComponent = static_cast<Camera*>(component);
			castedComponent->m_fovy = m_fovy;
			castedComponent->updateProjection();
		}
	}
	if (ImGui::SliderFloat("near", &(m_zNear), 0.001f, 5.f))
	{
		for (auto component : components)
		{
			Camera* castedComponent = static_cast<Camera*>(component);
			castedComponent->m_zNear = m_zNear;
			castedComponent->updateProjection();
		}
	}
	if (ImGui::SliderFloat("far", &(m_zFar), 0.01f, 1000.f))
	{
		for (auto component : components)
		{
			Camera* castedComponent = static_cast<Camera*>(component);
			castedComponent->m_zFar = m_zFar;
			castedComponent->updateProjection();
		}
	}
	if (ImGui::SliderFloat("aspect", &(m_aspect), 0.01f, 10.f))
	{
		for (auto component : components)
		{
			Camera* castedComponent = static_cast<Camera*>(component);
			castedComponent->m_aspect = m_aspect;
			castedComponent->updateProjection();
		}
	}
}

void Camera::eraseFromScene(Scene& scene)
{
	scene.erase(this);
}

void Camera::addToScene(Scene& scene)
{
	scene.add(this);
}

Component * Camera::clone(Entity* entity)
{
	Camera* camera = new Camera(*this);

	camera->attachToEntity(entity);

	return camera;
}

void Camera::eraseFromEntity(Entity & entity)
{
	entity.erase(this);
}

void Camera::addToEntity(Entity & entity)
{
	entity.add(this);
}

void Camera::updateScreenSize(float screenWidth, float screenHeight)
{
	m_aspect = screenWidth / screenHeight;
	
	//TODO 
	//also resize orthographic frustum ?

	updateProjection();
}

void Camera::setPerspectiveInfos(float fovy, float aspect, float zNear, float zFar)
{
	m_fovy = fovy;
	m_aspect = aspect;
	m_zNear = zNear;
	m_zFar = zFar;

	updateProjection();
}

void Camera::setOrthographicInfos(float left, float right, float bottom, float top, float zNear, float zFar)
{
	m_left = left;
	m_right = right;
	m_top = top;
	m_bottom = bottom;
	m_zNear = zNear;
	m_zFar = zFar;

	updateProjection();
}

void Camera::setCameraMode(CameraMode cameraMode)
{
	m_cameraMode = cameraMode;
	updateProjection();
}

glm::mat4 Camera::getViewMatrix() const
{
	return m_viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const
{
	return m_projectionMatrix;
}

glm::vec3 Camera::getCameraPosition() const
{
	return m_position;
}

glm::vec3 Camera::getCameraForward() const
{
	return m_forward;
}

void Camera::setFOV(float fov)
{
	m_fovy = fov;
}

void Camera::setNear(float camNear)
{
	m_zNear = camNear;
}

void Camera::setFar(float camFar)
{
	m_zFar = camFar;
}

void Camera::setAspect(float aspect)
{
	m_aspect = aspect;
}

float Camera::getFOV() const
{
	return m_fovy;
}

float Camera::getNear() const
{
	return m_zNear;
}

float Camera::getFar() const
{
	return m_zNear;
}

float Camera::getAspect() const
{
	return m_aspect;
}

void Camera::updateProjection()
{
	if (m_cameraMode == CameraMode::PERSPECTIVE)
		m_projectionMatrix = glm::perspective(m_fovy, m_aspect, m_zNear, m_zFar);
	else
		m_projectionMatrix = glm::ortho(m_left, m_right, m_bottom, m_top, m_zNear, m_zFar);
}

void Camera::save(Json::Value & rootComponent) const
{
	Component::save(rootComponent);

	rootComponent["cameraMode"] = (int)m_cameraMode;
	rootComponent["lookPosition"] = toJsonValue(m_lookPosition);
	rootComponent["position"] = toJsonValue(m_position);
	rootComponent["up"] = toJsonValue(m_up);
	rootComponent["forward"] = toJsonValue(m_forward);
	rootComponent["viewMatrix"] = toJsonValue(m_viewMatrix);
	rootComponent["projectionMatrix"] = toJsonValue(m_projectionMatrix);
	rootComponent["fovy"] = m_fovy;
	rootComponent["aspect"] = m_aspect;
	rootComponent["zNear"] = m_zNear;
	rootComponent["zFar"] = m_zFar;
	rootComponent["left"] = m_left;
	rootComponent["top"] = m_top;
	rootComponent["right"] = m_right;
	rootComponent["bottom"] = m_bottom;
}

void Camera::load(const Json::Value& rootComponent)
{
	Component::load(rootComponent);

	m_cameraMode = (CameraMode)rootComponent.get("cameraMode", CameraMode::PERSPECTIVE).asInt();
	m_lookPosition = fromJsonValue<glm::vec3>(rootComponent["lookPosition"], glm::vec3());
	m_position = fromJsonValue<glm::vec3>(rootComponent["position"], glm::vec3());
	m_up = fromJsonValue<glm::vec3>(rootComponent["up"], glm::vec3());
	m_forward = fromJsonValue<glm::vec3>(rootComponent["forward"], glm::vec3());
	m_viewMatrix = fromJsonValue<glm::mat4>(rootComponent["viewMatrix"], glm::mat4());
	m_projectionMatrix = fromJsonValue<glm::mat4>(rootComponent["projectionMatrix"], glm::mat4());
	m_fovy = rootComponent.get("fovy", 45).asFloat();
	m_aspect = rootComponent.get("aspect", 16.f/9.f).asFloat();
	m_zNear = rootComponent.get("zNear", 0.1f).asFloat();
	m_zFar = rootComponent.get("zFar", 500.f).asFloat();
	m_left = rootComponent.get("left", -10).asFloat();
	m_top = rootComponent.get("top", 10).asFloat();
	m_right = rootComponent.get("right", 10).asFloat();
	m_bottom = rootComponent.get("bottom", -10).asFloat();
}


//////////////////////////////////


CameraEditor::CameraEditor() : BaseCamera(), isFPSMode(false), radius(3.f), theta(0), phi(glm::pi<float>()*0.5f),
	o(0, 0, 0), eye(0, 0, -1), up(0, 1, 0), forward(0, 0, 1), right(1, 0, 0),
	m_fovy(45.0f), m_aspect(16.f / 9.f), m_zNear(0.1f), m_zFar(1000.f)
{
	updateProjection();
	updateTransform();
}

void CameraEditor::setTranslationLocal(glm::vec3 pos)
{
	if (isFPSMode)
	{
		eye = glm::vec3(0, 0, 0);
		eye += up * pos.y;
		eye += right * pos.x;
		eye += forward * pos.z;
	}
	else
	{
		o = glm::vec3(0, 0, 0);
		o = up * pos.y * radius * 2.f;
		o = -right * pos.x * radius * 2.f;
		o = forward * pos.z * radius * 2.f;
	}

	updateTransform();
}

void CameraEditor::translateLocal(glm::vec3 pos)
{

	if (isFPSMode)
	{
		eye += up * pos.y;
		eye += right * pos.x;
		eye += forward * pos.z;
	}
	else
	{
		o += up * pos.y * radius * 2.f;
		o += -right * pos.x * radius * 2.f;
		o += forward * pos.z * radius * 2.f;
	}

	updateTransform();
}

void CameraEditor::setDirection(glm::vec3 dir)
{
	forward = dir;

	updateTransform();
}

void CameraEditor::setRotation(float _phi, float _theta)
{
	theta = 1.f * _theta;
	phi = 1.f * _phi;

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
	if (isFPSMode)
		eye = pos;
	else
		o = pos;

	updateTransform();
}

void CameraEditor::translate(glm::vec3 pos)
{
	if (isFPSMode)
		eye += pos;
	else
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

	if (isFPSMode)
		o = eye + forward;
	else
		eye = o - forward*radius;

	m_viewMatrix = glm::lookAt(eye, o, up);
}

void CameraEditor::setFPSMode(bool fpsMode)
{
	isFPSMode = fpsMode;
	if (isFPSMode)
		o = eye + forward;
	else
		eye = o - forward*radius;
}

void CameraEditor::updateProjection()
{
	m_projectionMatrix = glm::perspective(m_fovy, m_aspect, m_zNear, m_zFar);
}

void CameraEditor::updateScreenSize(float screenWidth, float screenHeight)
{
	m_aspect = screenWidth / screenHeight;

	m_projectionMatrix = glm::perspective(m_fovy, m_aspect, m_zNear, m_zFar);
}

void CameraEditor::setPerspectiveInfos(float fovy, float aspect, float zNear , float zFar)
{
	m_fovy = fovy;
	m_aspect = aspect;
	m_zNear = zNear;
	m_zFar = zFar;

	m_projectionMatrix = glm::perspective(m_fovy, m_aspect, m_zNear, m_zFar);
}

void CameraEditor::setOrthographicInfos(float left, float right, float bottom, float top, float zNear, float zFar)
{
	//nothing
}

void CameraEditor::setCameraMode(CameraMode cameraMode)
{
	//nothing
}

glm::mat4 CameraEditor::getViewMatrix() const
{
	return m_viewMatrix;
}

glm::mat4 CameraEditor::getProjectionMatrix() const
{
	return m_projectionMatrix;
}

glm::vec3 CameraEditor::getCameraPosition() const
{
	return eye;
}

glm::vec3 CameraEditor::getCameraForward() const
{
	return forward;
}

void CameraEditor::setFOV(float fov)
{
	m_fovy = fov;
	updateProjection();
}

void CameraEditor::setNear(float camNear)
{
	m_zNear = camNear;
	updateProjection();
}

void CameraEditor::setFar(float camFar)
{
	m_zFar = camFar;
	updateProjection();
}

void CameraEditor::setAspect(float aspect)
{
	m_aspect = aspect;
	updateProjection();
}

float CameraEditor::getFOV() const
{
	return m_fovy;
}

float CameraEditor::getNear() const
{
	return m_zNear;
}

float CameraEditor::getFar() const
{
	return m_zFar;
}

float CameraEditor::getAspect() const
{
	return m_aspect;
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



///////////////////////////

/*
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

*/

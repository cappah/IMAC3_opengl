#pragma once

#include <math.h>

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Component.h"

struct Camera : public Component
{
	//float radius;
	float theta;
	float phi;
	glm::vec3 o;
	glm::vec3 eye;

	glm::vec3 up;
	glm::vec3 forward;
	glm::vec3 right;

	Camera();
	Camera(const Camera& other);

	virtual void setTranslationLocal(glm::vec3 pos) = 0;
	virtual void translateLocal(glm::vec3 pos) = 0;
	virtual void setDirection(glm::vec3 dir) = 0;
	virtual void setRotation(float phi, float theta) = 0;
	virtual void setTranslation(glm::vec3 pos) = 0;
	virtual void translate(glm::vec3 pos) = 0;
	virtual void rotate(float deltaX, float deltaY) = 0;
	virtual void updateTransform() = 0;

	virtual void drawUI() override;
	virtual void eraseFromScene(Scene & scene) override;
	virtual void addToScene(Scene & scene) override;
	virtual Component * clone(Entity * entity) = 0;

};

struct CameraFPS : public Camera
{

	CameraFPS();
	CameraFPS(const Camera& cam);

	virtual void setTranslationLocal(glm::vec3 pos) override;
	virtual void translateLocal(glm::vec3 pos) override;
	virtual void setDirection(glm::vec3 dir) override;
	virtual void setRotation(float phi, float theta) override;
	virtual void setTranslation(glm::vec3 pos) override;
	virtual void translate(glm::vec3 pos) override;
	virtual void rotate(float deltaX, float deltaY) override;
	virtual void updateTransform() override;
	void switchFromCameraEditor(const Camera& other); // make the transition between camera editor and camera fps

	virtual Component * clone(Entity * entity) override;

};

struct CameraEditor : public Camera
{
	float radius;

	CameraEditor();
	CameraEditor(const Camera& cam);

	virtual void setTranslationLocal(glm::vec3 pos) override;
	virtual void translateLocal(glm::vec3 pos) override;
	virtual void setDirection(glm::vec3 dir) override;
	virtual void setRotation(float phi, float theta) override;
	virtual void setTranslation(glm::vec3 pos) override;
	virtual void translate(glm::vec3 pos) override;
	virtual void rotate(float deltaX, float deltaY) override;
	virtual void updateTransform() override;
	void switchFromCameraFPS(const Camera& other);

	virtual Component * clone(Entity * entity) override;

};

/*
void camera_defaults(Camera & c);
void camera_zoom(Camera & c, float factor);
void camera_turn(Camera & c, float phi, float theta);
void camera_pan(Camera & c, float x, float y);
void camera_rotate(Camera& c, float phi, float theta);
void camera_translate(Camera& c, float x, float y, float z);
void toogleCamera(Camera& c);
*/
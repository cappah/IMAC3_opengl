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
#include "Octree.h"
#include "IRenderableComponent.h"
#include "RenderBatch.h"
#include "PostProcess.h"

class Skybox;

struct BaseCamera
{
public:
	enum ClearMode {
		COLOR,
		SKYBOX
	};

protected:

	glm::vec3 m_position;
	glm::vec3 m_forward;

	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;

	float m_fovy;
	float m_aspect;
	float m_zNear;
	float m_zFar;

	std::map<GLuint, std::shared_ptr<IRenderBatch>> m_renderBatches[PipelineTypes::COUNT];
	PostProcessProxy m_postProcessProxy;
	GlHelper::Framebuffer m_frameBuffer;
	Texture m_texture;
	MaterialBlit* m_material;
	Mesh* m_quadMesh;

	glm::vec4 m_clearColor;
	std::shared_ptr<Skybox> m_skybox;
	ClearMode m_clearMode;


public:
	enum CameraMode { PERSPECTIVE, ORTHOGRAPHIC };

	BaseCamera();

	virtual void computeCulling(const Octree<IRenderableComponent, AABB>& octree);
	virtual const std::map<GLuint, std::shared_ptr<IRenderBatch>>& getRenderBatches(PipelineTypes renderPipelineType) const;
	virtual const PostProcessProxy& getPostProcessProxy() const;
	virtual void onViewportResized(const glm::vec2& newSize);
	void renderFrame(const Texture* texture);
	const Texture* getFinalFrame() const;
	const GlHelper::Framebuffer& getFrameBuffer();

	ClearMode getClearMode() const;
	const glm::vec4& getClearColor() const;
	void renderSkybox() const;

	//virtual void setTranslationLocal(glm::vec3 pos) = 0;
	//virtual void translateLocal(glm::vec3 pos) = 0;
	//virtual void setDirection(glm::vec3 dir) = 0;
	//virtual void setRotation(float phi, float theta) = 0;
	//virtual void setTranslation(glm::vec3 pos) = 0;
	//virtual void translate(glm::vec3 pos) = 0;
	//virtual void rotate(float deltaX, float deltaY) = 0;
	//virtual void updateTransform() = 0;

	virtual void updateScreenSize(float screenWidth, float screenHeight) = 0;
	virtual void setPerspectiveInfos(float fovy, float aspect, float camNear = 0.1f, float camFar = 100.f) = 0;
	virtual void setOrthographicInfos(float left, float right, float bottom, float top, float zNear, float zFar) = 0;
	virtual void setCameraMode(CameraMode cameraMode) = 0;
	virtual void updateProjection() = 0;

	const glm::mat4& getViewMatrix() const;
	const glm::mat4& getProjectionMatrix() const;
	const glm::vec3& getCameraPosition() const;
	const glm::vec3& getCameraForward() const;

	void setFOV(float fov);
	void setNear(float camNear);
	void setFar(float camFar);
	void setAspect(float aspect);

	float getFOV() const;
	float getNear() const;
	float getFar() const;
	float getAspect() const;
};

class Camera : public Component, public BaseCamera
{
	REFLEXION_HEADER(Camera)
	COMPONENT_IMPLEMENTATION_HEADER(Camera)

private :
	CameraMode m_cameraMode;

	glm::vec3 m_lookPosition;
	glm::vec3 m_up;

	//glm::vec3 m_position;
	//glm::vec3 m_forward;

	//glm::mat4 m_viewMatrix;
	//glm::mat4 m_projectionMatrix;

	//float m_fovy;
	//float m_aspect;
	//float m_zNear;
	//float m_zFar;

	float m_left;
	float m_top;
	float m_right;
	float m_bottom;

	//std::map<GLuint, std::shared_ptr<IRenderBatch>> m_renderBatches[PipelineTypes::COUNT];
	//PostProcessProxy m_postProcessProxy;
	//GlHelper::Framebuffer m_frameBuffer;
	//Texture m_texture;
	//MaterialBlit* m_material;
	//Mesh* m_quadMesh;

public:

	Camera();

	//virtual void computeCulling(const Octree<IRenderableComponent, AABB>& octree) override;
	//virtual const std::map<GLuint, std::shared_ptr<IRenderBatch>>& getRenderBatches(PipelineTypes renderPipelineType) const override;
	//virtual const PostProcessProxy& getPostProcessProxy() const override;

	//void onViewportResized(float width, float height);
	//void renderFrame(const Texture* texture);

	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1, 1, 1), const glm::quat& rotation = glm::quat()) override;

	virtual void drawInInspector(Scene& scene) override;
	virtual void drawInInspector(Scene& scene, const std::vector<Component*>& components) override;
	
	virtual void updateScreenSize(float screenWidth, float screenHeight) override;
	virtual void setPerspectiveInfos(float fovy, float aspect, float zNear = 0.1f, float zFar = 100.f) override;
	virtual void setOrthographicInfos(float left, float right, float bottom, float top, float zNear, float zFar) override;
	virtual void setCameraMode(CameraMode cameraMode) override;

	//virtual const glm::mat4& getViewMatrix() const override;
	//virtual const glm::mat4& getProjectionMatrix() const override;
	//virtual const glm::vec3& getCameraPosition() const override;
	//virtual const glm::vec3& getCameraForward() const override;

	//virtual void setFOV(float fov) override;
	//virtual void setNear(float camNear) override;
	//virtual void setFar(float camFar) override;
	//virtual void setAspect(float aspect) override;
	//virtual float getFOV() const override;
	//virtual float getNear() const override;
	//virtual float getFar() const override;
	//virtual float getAspect() const override;

	void updateProjection();

	virtual void save(Json::Value& rootComponent) const override;
	virtual void load(const Json::Value& rootComponent) override;
};

REFLEXION_CPP(Camera)
REFLEXION_InheritFrom(Camera, Component)

class CameraEditor final : public BaseCamera
{
private:
	float radius;
	float theta;
	float phi;
	glm::vec3 o;

	//glm::vec3 eye;
	//glm::vec3 forward;

	glm::vec3 up;
	glm::vec3 right;

	bool m_isFPSMode;

	//float m_fovy;
	//float m_aspect;
	//float m_zNear;
	//float m_zFar;

	//glm::mat4 m_viewMatrix;
	//glm::mat4 m_projectionMatrix;

	//std::map<GLuint, std::shared_ptr<IRenderBatch>> m_renderBatches[PipelineTypes::COUNT];
	//PostProcessProxy m_postProcessProxy;

	float m_cameraBaseSpeed;
	float m_cameraBoostSpeed;
	bool m_hideCursorWhenMovingCamera;

	GlHelper::Renderbuffer m_depthBuffer;

public:
	CameraEditor();

	//void computeCulling(const Octree<IRenderableComponent, AABB>& octree) override;
	//const std::map<GLuint, std::shared_ptr<IRenderBatch>>& getRenderBatches(PipelineTypes renderPipelineType) const override;
	//virtual const PostProcessProxy& getPostProcessProxy() const override;

	virtual void onViewportResized(const glm::vec2& newSize) override;

	void drawUI();
	float getCameraBaseSpeed() const;
	float getCameraBoostSpeed() const;
	bool getFPSMode() const;

	void setTranslationLocal(glm::vec3 pos);
	void translateLocal(glm::vec3 pos);
	void setDirection(glm::vec3 dir);
	void setRotation(float phi, float theta);
	void setTranslation(glm::vec3 pos);
	void translate(glm::vec3 pos);
	void rotate(float deltaX, float deltaY);
	void updateTransform();
	void setFPSMode(bool fpsMode);

	void updateProjection();

	virtual void updateScreenSize(float screenWidth, float screenHeight) override;
	virtual void setPerspectiveInfos(float fovy, float aspect, float near = 0.1f, float = 100.f) override;
	virtual void setOrthographicInfos(float left, float right, float bottom, float top, float zNear, float zFar) override;
	virtual void setCameraMode(CameraMode cameraMode) override;

	//virtual const glm::mat4& getViewMatrix() const override;
	//virtual const glm::mat4& getProjectionMatrix() const override;
	//virtual const glm::vec3& getCameraPosition() const override;
	//virtual const glm::vec3& getCameraForward() const override;

	//// Hérité via BaseCamera
	//virtual void setFOV(float fov) override;
	//virtual void setNear(float near) override;
	//virtual void setFar(float far) override;
	//virtual void setAspect(float aspect) override;
	//virtual float getFOV() const override;
	//virtual float getNear() const override;
	//virtual float getFar() const override;
	//virtual float getAspect() const override;

};


/*
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
*/

/*
void camera_defaults(Camera & c);
void camera_zoom(Camera & c, float factor);
void camera_turn(Camera & c, float phi, float theta);
void camera_pan(Camera & c, float x, float y);
void camera_rotate(Camera& c, float phi, float theta);
void camera_translate(Camera& c, float x, float y, float z);
void toogleCamera(Camera& c);
*/
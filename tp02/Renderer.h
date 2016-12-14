#pragma once

#include "glew/glew.h"

#include "Mesh.h"
#include "LightManager.h"
#include "Application.h"
#include "Entity.h"
#include "Collider.h"
#include "Terrain.h"
#include "Skybox.h"
#include "Materials.h"
#include "Flag.h"
#include "ParticleEmitter.h"
#include "FrameBuffer.h"
#include "ErrorHandler.h"

#include "PostProcess.h"

class DebugDrawRenderer;

struct LightCullingInfo
{
	glm::vec4 viewport;
	int idx;

	inline LightCullingInfo(const glm::vec4& _viewport, int _idx) : viewport(_viewport), idx(_idx) {}
};

class Renderer
{
	enum LightType { POINT = 0, DIRECTIONAL = 1, SPOT = 2 };

private:
	// Light pass FrameBuffer :
	Texture* m_lightPassHDRColor;
	Texture* m_lightPassDepth;
	Texture* m_lightPassHighValues;
	GlHelper::Framebuffer m_lightPassBuffer;
	glm::vec2 m_viewportRenderSize;

	std::shared_ptr<MaterialShadowPass> shadowPassMaterial;
	std::shared_ptr<MaterialShadowPassOmni> shadowPassOmniMaterial;

	//uniform for unidirectional shadow map
	GLuint uniformShadowMVP;

	//uniforms for omniDirectional shadow map : 
	GLuint uniformShadowOmniModelMatrix;
	GLuint uniformShadowOmniVPLight[6];
	GLuint uniformShadowOmniFarPlane;
	GLuint uniformShadowOmniLightPos;

	//lighting materials : 
	std::shared_ptr<MaterialPointLight> m_pointLightMaterial;
	std::shared_ptr<MaterialDirectionalLight> m_directionalLightMaterial;
	std::shared_ptr<MaterialSpotLight> m_spotLightMaterial;

	Mesh quadMesh;

	//light system
	LightManager* lightManager;

	GlHelper::Framebuffer gBufferFBO;
	Texture gPassColorTexture;
	Texture gPassNormalTexture;
	Texture gPassDepthTexture;
	Texture gPassHightValuesTexture;

	//for blit pass 
	std::shared_ptr<MaterialBlit> glProgram_blit;

	//light count after culling : 
	int pointLightCount;
	int spotLightCount;

	//for light culling : 
	std::vector<LightCullingInfo> pointLightCullingInfos;
	std::vector<LightCullingInfo> spotLightCullingInfos;

	//post process : 
	PostProcessManager m_postProcessManager;

	////shadows : 
	//GLuint shadowFrameBuffer;
	//GLuint shadowRenderBuffer;
	//GLuint shadowTexture;

public:
	Renderer(LightManager* _lightManager, std::string programGPass_vert_path, std::string programGPass_frag_path, std::string programLightPass_vert_path, std::string programLightPass_frag_path_pointLight, std::string programLightPass_frag_path_directionalLight, std::string programLightPass_frag_path_spotLight);
	~Renderer();

	//return the final frame, after all render process
	Texture* getFinalFrame() const;
	const glm::vec2& getViewportRenderSize() const;

	//update deferred textures used by the FBO when we resize the screen.
	void onResizeViewport(const glm::vec2& newViewportSize);

	//initialyze buffers for blit quad.
	void initPostProcessQuad(std::string programBlit_vert_path, std::string programBlit_frag_path);

	void initialyzeShadowMapping(std::string progamShadowPass_vert_path, std::string progamShadowPass_frag_path, std::string progamShadowPassOmni_vert_path, std::string progamShadowPassOmni_frag_path, std::string progamShadowPassOmni_geom_path);

	//render a shadow on a shadow map
	void renderShadows(const glm::mat4& lightProjection, const glm::mat4& lightView, const IDrawable& drawable);

	//render a shadow on a shadow map
	void renderShadows(float farPlane, const glm::vec3 & lightPos, const std::vector<glm::mat4>& lightVPs, const IDrawable& drawable);

	//render all entities of the scene, using deferred shading.
	//[DEPRECATED]
	//void render(const BaseCamera& camera, std::vector<MeshRenderer*>& meshRenderers, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights, Terrain& terrain, Skybox& skybox, std::vector<Physic::Flag*>& flags, std::vector<Billboard*>& billboards, std::vector<Physic::ParticleEmitter*>& particleEmitters, DebugDrawRenderer* debugDrawer);
	void render(BaseCamera& camera, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights, DebugDrawRenderer* debugDrawer = nullptr);
	void renderLightedScene(const BaseCamera& camera, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights, DebugDrawRenderer* debugDrawer = nullptr);
	void shadowPass(const BaseCamera& camera, const std::map<GLuint, std::shared_ptr<IRenderBatch>>& opaqueRenderBatches, const std::map<GLuint, std::shared_ptr<IRenderBatch>>& transparentRenderBatches, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights);
	void gPass(const std::map<GLuint, std::shared_ptr<IRenderBatch>>& opaqueRenderBatches, const glm::mat4& projection, const glm::mat4& view);
	void lightPass(const glm::mat4& screenToWorld, const glm::vec3& cameraPosition, const glm::vec3& cameraForward, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights);
	void deferredPipeline(const std::map<GLuint, std::shared_ptr<IRenderBatch>>& opaqueRenderBatches, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& cameraPosition, const glm::vec3& cameraForward, const glm::mat4& screenToWorld, const glm::mat4& camera_mvp, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights);
	void forwardPipeline(const std::map<GLuint, std::shared_ptr<IRenderBatch>>& transparentRenderBatches, int width, int height, const glm::mat4& projection, const glm::mat4& view);

	void debugDrawRenderer(DebugDrawRenderer& debugDrawer) const;

	void transferDepthTo(const GlHelper::Framebuffer& to, const glm::vec2& depthTextureSize) const;

	//draw colliders on scene.
	void debugDrawColliders(const BaseCamera& camera, const std::vector<Entity*>& entities);

	//draw textures of gPass.
	//void debugDrawDeferred();

	//draw lights bounding box.
	void debugDrawLights(const BaseCamera& camera, const std::vector<PointLight*>& pointLights, const std::vector<SpotLight*>& spotLights);

	//check if a light bounding box has to be drawn, in that case it scales the blit quad to render only what is influenced by the light.
	bool passCullingTest(glm::vec4& viewport, const glm::mat4& projection, const glm::mat4& view, const glm::vec3 cameraPosition, BoxCollider& collider);

	//resize the blit quad, changing its vertices coordinates
	void resizeBlitQuad(const glm::vec4& viewport = glm::vec4(-1,-1,2,2));

	// Camera culling for light
	void updateCulling(const BaseCamera& camera, std::vector<PointLight*>& pointLights, std::vector<SpotLight*>& spotLights, std::vector<LightCullingInfo>& pointLightCullingInfos, std::vector<LightCullingInfo>& spotLightCullingInfos);
};


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

	//frame buffer for deferred lighting
	GLuint gbufferFbo;
	GLuint gbufferTextures[3];

	//for blit pass 
	std::shared_ptr<MaterialBlit> glProgram_blit;

	//light count after culling : 
	int pointLightCount;
	int spotLightCount;

	//for light culling : 
	std::vector<LightCullingInfo> pointLightCullingInfos;
	std::vector<LightCullingInfo> spotLightCullingInfos;

	////shadows : 
	//GLuint shadowFrameBuffer;
	//GLuint shadowRenderBuffer;
	//GLuint shadowTexture;

public:
	Renderer(LightManager* _lightManager, std::string programGPass_vert_path, std::string programGPass_frag_path, std::string programLightPass_vert_path, std::string programLightPass_frag_path_pointLight, std::string programLightPass_frag_path_directionalLight, std::string programLightPass_frag_path_spotLight);
	~Renderer();

	//update deferred textures used by the FBO when we resize the screen.
	void onResizeWindow();

	//initialyze buffers for blit quad.
	void initPostProcessQuad(std::string programBlit_vert_path, std::string programBlit_frag_path);

	void initialyzeShadowMapping(std::string progamShadowPass_vert_path, std::string progamShadowPass_frag_path, std::string progamShadowPassOmni_vert_path, std::string progamShadowPassOmni_frag_path, std::string progamShadowPassOmni_geom_path);

	//render a shadow on a shadow map
	void renderShadows(const glm::mat4& lightProjection, const glm::mat4& lightView, MeshRenderer& meshRenderer);

	//render a shadow on a shadow map
	void renderShadows(float farPlane, const glm::vec3 & lightPos, const std::vector<glm::mat4>& lightVPs, MeshRenderer & meshRenderer);

	//render all entities of the scene, using deferred shading.
	void render(const BaseCamera& camera, std::vector<MeshRenderer*>& meshRenderers, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights, Terrain& terrain, Skybox& skybox, std::vector<Physic::Flag*>& flags, std::vector<Billboard*>& billboards, std::vector<Physic::ParticleEmitter*>& particleEmitters);

	//draw colliders on scene.
	void debugDrawColliders(const BaseCamera& camera, const std::vector<Entity*>& entities);

	//draw textures of gPass.
	void debugDrawDeferred();

	//draw lights bounding box.
	void debugDrawLights(const BaseCamera& camera, const std::vector<PointLight*>& pointLights, const std::vector<SpotLight*>& spotLights);

	//check if a light bounding box has to be drawn, in that case it scales the blit quad to render only what is influenced by the light.
	bool passCullingTest(glm::vec4& viewport, const glm::mat4& projection, const glm::mat4& view, const glm::vec3 cameraPosition, BoxCollider& collider);

	//resize the blit quad, changing its vertices coordinates
	void resizeBlitQuad(const glm::vec4& viewport = glm::vec4(-1,-1,2,2));

	// Camera culling for light
	void updateCulling(const BaseCamera& camera, std::vector<PointLight*>& pointLights, std::vector<SpotLight*>& spotLights, std::vector<LightCullingInfo>& pointLightCullingInfos, std::vector<LightCullingInfo>& spotLightCullingInfos);
};


#pragma once

#include "glew/glew.h"

#include "Mesh.h"
#include "LightManager.h"
#include "Application.h"
#include "Entity.h"
#include "Collider.h"
#include "Terrain.h"



class Renderer
{
	enum LightType { POINT = 0, DIRECTIONAL = 1, SPOT = 2 };

private:
	GLuint glProgram_gPass;
	GLuint glProgram_lightPass_pointLight;
	GLuint glProgram_lightPass_directionalLight;
	GLuint glProgram_lightPass_spotLight;

	GLuint uniformTexturePosition[3];
	GLuint uniformTextureNormal[3];
	GLuint uniformTextureDepth[3];
	GLuint unformScreenToWorld[3];
	GLuint uniformCameraPosition[3];

	Mesh quadMesh;

	//light system
	LightManager* lightManager;

	//frame buffer for deferred lighting
	GLuint gbufferFbo;
	GLuint gbufferTextures[3];

	//for blit pass 
	GLuint glProgram_blit;
	GLuint uniformTextureBlit;

	//light count after culling : 
	int pointLightCount;
	int spotLightCount;

public:
	Renderer(LightManager* _lightManager, std::string programGPass_vert_path, std::string programGPass_frag_path, std::string programLightPass_vert_path, std::string programLightPass_frag_path_pointLight, std::string programLightPass_frag_path_directionalLight, std::string programLightPass_frag_path_spotLight);

	void initPostProcessQuad(std::string programBlit_vert_path, std::string programBlit_frag_path);

	//void render(Camera& camera, std::vector<Entity*> entities);

	void render(const Camera& camera, std::vector<MeshRenderer*>& meshRenderers, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights, Terrain& terrain);

	//draw colliders on scene
	void debugDrawColliders(const Camera& camera, const std::vector<Entity*>& entities);

	//draw textures of gPass
	void debugDrawDeferred();

	//draw lights bounding box
	void debugDrawLights(const Camera& camera, const std::vector<PointLight*>& pointLights, const std::vector<SpotLight*>& spotLights);

	void updateCulling(const Camera& camera, std::vector<PointLight*>& pointLights, std::vector<SpotLight*>& spotLights);
};


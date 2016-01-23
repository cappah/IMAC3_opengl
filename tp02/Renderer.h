#pragma once

#include "glew/glew.h"

#include "Mesh.h"
#include "LightManager.h"
#include "Application.h"
#include "Entity.h"


class Renderer
{
private:
	GLuint glProgram_gPass;
	GLuint glProgram_lightPass;

	GLuint uniformTexturePosition;
	GLuint uniformTextureNormal;
	GLuint uniformTextureDepth;
	GLuint unformScreenToWorld;
	GLuint uniformCameraPosition;

	Mesh quadMesh;

	//light system
	LightManager* lightManager;

	//frame buffer for deferred lighting
	GLuint gbufferFbo;
	GLuint gbufferTextures[3];

	//for blit pass 
	GLuint glProgram_blit;
	GLuint uniformTextureBlit;

public:
	Renderer(LightManager* _lightManager, std::string programGPass_vert_path, std::string programGPass_frag_path, std::string programLightPass_vert_path, std::string programLightPass_frag_path);

	void initPostProcessQuad(std::string programBlit_vert_path, std::string programBlit_frag_path);

	//void render(Camera& camera, std::vector<Entity*> entities);

	void render(const Camera& camera, std::vector<MeshRenderer*>& meshRenderers, std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights);

	//draw colliders on scene
	void debugDrawColliders(const Camera& camera, const std::vector<Entity*>& entities);

	//draw textures of gPass
	void debugDrawDeferred();
};


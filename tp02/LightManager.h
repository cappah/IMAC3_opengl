#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "Lights.h"

struct ShadowMap
{
	int textureWidth;
	int textureHeight;

	GLuint shadowFrameBuffer;
	GLuint shadowRenderBuffer;
	GLuint shadowTexture;

	ShadowMap(int _textureWidth = 1024, int _textureHeight = 1024);
	~ShadowMap();
};

struct OmniShadowMap // omnidirectional shadow map for point lights
{
	int textureWidth;
	int textureHeight;

	GLuint shadowFrameBuffer;
	GLuint shadowRenderBuffer;
	GLuint shadowTexture;

	OmniShadowMap(int _textureWidth = 1024, int _textureHeight = 1024);
	~OmniShadowMap();
};

class LightManager
{
public:
	enum LightType {POINT, DIRECTIONAL, SPOT};

private:

	GLuint uniform_pointLight_pos;
	GLuint uniform_pointLight_col;
	GLuint uniform_pointLight_int;

	GLuint uniform_directionalLight_dir;
	GLuint uniform_directionalLight_col;
	GLuint uniform_directionalLight_int;

	GLuint uniform_spotLight_dir;
	GLuint uniform_spotLight_col;
	GLuint uniform_spotLight_int;
	GLuint uniform_spotLight_pos;
	GLuint uniform_spotLight_angle;

	//shadows : 
	std::vector<ShadowMap> spot_shadowMaps;
	std::vector<ShadowMap> directional_shadowMaps;
	std::vector<OmniShadowMap> point_shadowMaps;

	float directionalShadowMapViewportSize;
	float directionalShadowMapViewportNear;
	float directionalShadowMapViewportFar;


public:
	LightManager();
	
	void init(GLuint glProgram_pointLight, GLuint glProgram_directionalLight, GLuint glProgram_spotLight);

	void setShadowMapCount(LightType lightType, unsigned int count);
	int getShadowMapCount(LightType lightType);
	//bind shadow map and resize viewport to cover the right area on screen
	void bindShadowMapFBO(LightType lightType, int index);
	void unbindShadowMapFBO(LightType lightType);
	void bindShadowMapTexture(LightType lightType, int index);


	void uniformPointLight(PointLight& light);
	void uniformDirectionalLight(DirectionalLight& light);
	void uniformSpotLight(SpotLight& light);

	float getDirectionalShadowMapViewportSize() const;
	float getDirectionalShadowMapViewportNear() const;
	float getDirectionalShadowMapViewportFar() const;
};


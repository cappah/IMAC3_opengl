#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "Materials.h"
#include "Lights.h"

struct ShadowMap
{
	int textureWidth;
	int textureHeight;

	GLuint shadowFrameBuffer;
	GLuint shadowRenderBuffer;
	GLuint shadowTexture;

	ShadowMap(int _textureWidth = 1024, int _textureHeight = 1024);
	ShadowMap(ShadowMap&& other) noexcept;
	ShadowMap& operator=(ShadowMap&& other) noexcept;
	~ShadowMap();

	ShadowMap(const ShadowMap& other) = delete;
	ShadowMap& operator=(const ShadowMap& other) = delete;
};

struct OmniShadowMap // omnidirectional shadow map for point lights
{
	int textureWidth;
	int textureHeight;

	GLuint shadowFrameBuffer;
	GLuint shadowRenderBuffer;
	GLuint shadowTexture;

	OmniShadowMap(int _textureWidth = 1024, int _textureHeight = 1024);
	OmniShadowMap(OmniShadowMap&& other) noexcept;
	OmniShadowMap& operator=(OmniShadowMap&& other) noexcept;
	~OmniShadowMap();

	OmniShadowMap(const OmniShadowMap& other) = delete;
	OmniShadowMap& operator=(const OmniShadowMap& other) = delete;
};

class LightManager
{
public:
	enum LightType {POINT, DIRECTIONAL, SPOT};

private:

	std::shared_ptr<MaterialPointLight> m_pointLightMaterial;
	std::shared_ptr<MaterialDirectionalLight> m_directionalLightMaterial;
	std::shared_ptr<MaterialSpotLight> m_spotLightMaterial;

	//shadows : 
	std::vector<ShadowMap> spot_shadowMaps;
	std::vector<ShadowMap> directional_shadowMaps;
	std::vector<OmniShadowMap> point_shadowMaps;

	float directionalShadowMapViewportSize;
	float directionalShadowMapViewportNear;
	float directionalShadowMapViewportFar;


public:
	LightManager();

	void setShadowMapCount(LightType lightType, unsigned int count);
	int getShadowMapCount(LightType lightType);
	//bind shadow map and resize viewport to cover the right area on screen
	void bindShadowMapFBO(LightType lightType, int index);
	void unbindShadowMapFBO(LightType lightType);
	void bindShadowMapTexture(LightType lightType, int index);

	void setLightingMaterials(std::shared_ptr<MaterialPointLight> pointLightMat, std::shared_ptr<MaterialDirectionalLight> directionalLightMat, std::shared_ptr<MaterialSpotLight> spotLightMat);
	void uniformPointLight(PointLight& light);
	void uniformDirectionalLight(DirectionalLight& light);
	void uniformSpotLight(SpotLight& light);

	float getDirectionalShadowMapViewportSize() const;
	float getDirectionalShadowMapViewportNear() const;
	float getDirectionalShadowMapViewportFar() const;
};


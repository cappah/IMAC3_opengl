#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "Lights.h"

class LightManager
{
private:
	/*
	std::vector<PointLight> pointLights;
	std::vector<DirectionalLight> directionalLights;
	std::vector<SpotLight> spotLights;
	
	float globalIntensity;
	float globalAngle;
	*/

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

public:
	LightManager();
	/*
	void addPointLight(PointLight light);

	void addDirectionalLight(DirectionalLight light);

	void addSpotLight(SpotLight light);

	void removePointLight(int index);

	void removeDirectionalLight(int index);

	void removeSpotLight(int index);

	void changeAllLightIntensities(float _intensity);

	void changeAllSpotAngle(float _angle);

	void drawUI();

	void renderLights();*/

	void init(GLuint glProgram_pointLight, GLuint glProgram_directionalLight, GLuint glProgram_spotLight);

	//void renderLights(std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights);

	void uniformPointLight(PointLight& light);
	void uniformDirectionalLight(DirectionalLight& light);
	void uniformSpotLight(SpotLight& light);
};


#include "LightManager.h"

LightManager::LightManager() //: globalIntensity(0.f)
{

}

/*
void LightManager::addPointLight(PointLight light)
{
	pointLights.push_back(light);
}

void LightManager::addDirectionalLight(DirectionalLight light)
{
	directionalLights.push_back(light);
}

void LightManager::addSpotLight(SpotLight light)
{
	spotLights.push_back(light);
}

void LightManager::removePointLight(int index)
{
	pointLights.erase(pointLights.begin() + index);
}

void LightManager::removeDirectionalLight(int index)
{
	directionalLights.erase(directionalLights.begin() + index);
}

void LightManager::removeSpotLight(int index)
{
	spotLights.erase(spotLights.begin() + index);
}

void LightManager::changeAllLightIntensities(float _intensity)
{
	for (int i = 0; i < pointLights.size(); i++)
	{
		pointLights[i].intensity = _intensity;
	}

	for (int i = 0; i < directionalLights.size(); i++)
	{
		directionalLights[i].intensity = _intensity*0.1f;
	}

	for (int i = 0; i < spotLights.size(); i++)
	{
		spotLights[i].intensity = _intensity;
	}
}

void LightManager::changeAllSpotAngle(float _angle)
{
	for (int i = 0; i < spotLights.size(); i++)
	{
		spotLights[i].angle = glm::radians(_angle);
	}
}

void LightManager::drawUI()
{
	if (ImGui::Button("add pointLight"))
		addPointLight(PointLight(10, glm::vec3(1, 1, 1), glm::vec3(0, 1, 0)));

	ImGui::SameLine();
	if (ImGui::Button("add DirectionalLight"))
		addDirectionalLight(DirectionalLight(10, glm::vec3(1, 1, 1), glm::vec3(0, -1, 0)));

	ImGui::SameLine();
	if (ImGui::Button("add spotLight"))
		addSpotLight(SpotLight(10, glm::vec3(1, 1, 1), glm::vec3(0, 1, 0), glm::vec3(0, -1, 0), glm::radians(30.f)));


	int lightCount = 0;
	for (auto& light : spotLights)
	{
		light.drawUI(lightCount);
		lightCount++;
	}
	lightCount = 0;
	for (auto& light : directionalLights)
	{
		light.drawUI(lightCount);
		lightCount++;
	}
	lightCount = 0;
	for (auto& light : pointLights)
	{
		light.drawUI(lightCount);
		lightCount++;
	}
}

void LightManager::renderLights()
{
	glUniform1i(uniform_pointLight_count, pointLights.size());
	glUniform1i(uniform_directionalLight_count, directionalLights.size());
	glUniform1i(uniform_spotLight_count, spotLights.size());

	for (int i = 0; i < pointLights.size(); i++)
	{
		glUniform3fv(uniform_pointLight_pos[i], 1, glm::value_ptr(pointLights[i].position));
		glUniform3fv(uniform_pointLight_col[i], 1, glm::value_ptr(pointLights[i].color));
		glUniform1f(uniform_pointLight_int[i], pointLights[i].intensity);
	}

	for (int i = 0; i < directionalLights.size(); i++)
	{
		glUniform3fv(uniform_directionalLight_dir[i], 1, glm::value_ptr(directionalLights[i].direction));
		glUniform3fv(uniform_directionalLight_col[i], 1, glm::value_ptr(directionalLights[i].color));
		glUniform1f(uniform_directionalLight_int[i], directionalLights[i].intensity);
	}

	for (int i = 0; i < spotLights.size(); i++)
	{
		glUniform3fv(uniform_spotLight_dir[i], 1, glm::value_ptr(spotLights[i].direction));
		glUniform3fv(uniform_spotLight_col[i], 1, glm::value_ptr(spotLights[i].color));
		glUniform1f(uniform_spotLight_int[i], spotLights[i].intensity);
		glUniform3fv(uniform_spotLight_pos[i], 1, glm::value_ptr(spotLights[i].position));
		glUniform1f(uniform_spotLight_angle[i], spotLights[i].angle);
	}
}
*/


void LightManager::init(GLuint glProgram_pointLight, GLuint glProgram_directionalLight, GLuint glProgram_spotLight)
{
	uniform_pointLight_pos = glGetUniformLocation(glProgram_pointLight, "pointLight.position");
	uniform_pointLight_col = glGetUniformLocation(glProgram_pointLight, "pointLight.color");
	uniform_pointLight_int = glGetUniformLocation(glProgram_pointLight, "pointLight.intensity");

	uniform_directionalLight_dir = glGetUniformLocation(glProgram_directionalLight, "directionalLight.direction");
	uniform_directionalLight_col = glGetUniformLocation(glProgram_directionalLight, "directionalLight.color");
	uniform_directionalLight_int = glGetUniformLocation(glProgram_directionalLight, "directionalLight.intensity");

	uniform_spotLight_dir = glGetUniformLocation(glProgram_spotLight, "spotLight.direction");
	uniform_spotLight_col = glGetUniformLocation(glProgram_spotLight, "spotLight.color");
	uniform_spotLight_int = glGetUniformLocation(glProgram_spotLight, "spotLight.intensity");
	uniform_spotLight_pos = glGetUniformLocation(glProgram_spotLight, "spotLight.position");
	uniform_spotLight_angle = glGetUniformLocation(glProgram_spotLight, "spotLight.angle");
}
/*
void LightManager::renderLights(std::vector<PointLight*>& pointLights, std::vector<DirectionalLight*>& directionalLights, std::vector<SpotLight*>& spotLights)
{

		glUniform3fv(uniform_pointLight_pos[i], 1, glm::value_ptr(pointLights[i]->position));
		glUniform3fv(uniform_pointLight_col[i], 1, glm::value_ptr(pointLights[i]->color));
		glUniform1f(uniform_pointLight_int[i], pointLights[i]->intensity);


		glUniform3fv(uniform_directionalLight_dir[i], 1, glm::value_ptr(directionalLights[i]->direction));
		glUniform3fv(uniform_directionalLight_col[i], 1, glm::value_ptr(directionalLights[i]->color));
		glUniform1f(uniform_directionalLight_int[i], directionalLights[i]->intensity);


		glUniform3fv(uniform_spotLight_dir[i], 1, glm::value_ptr(spotLights[i]->direction));
		glUniform3fv(uniform_spotLight_col[i], 1, glm::value_ptr(spotLights[i]->color));
		glUniform1f(uniform_spotLight_int[i], spotLights[i]->intensity);
		glUniform3fv(uniform_spotLight_pos[i], 1, glm::value_ptr(spotLights[i]->position));
		glUniform1f(uniform_spotLight_angle[i], spotLights[i]->angle);
}
*/

void LightManager::uniformPointLight(PointLight & light)
{
	glUniform3fv(uniform_pointLight_pos, 1, glm::value_ptr(light.position));
	glUniform3fv(uniform_pointLight_col, 1, glm::value_ptr(light.color));
	glUniform1f(uniform_pointLight_int, light.intensity);
}

void LightManager::uniformDirectionalLight(DirectionalLight & light)
{
	glUniform3fv(uniform_directionalLight_dir, 1, glm::value_ptr(light.direction));
	glUniform3fv(uniform_directionalLight_col, 1, glm::value_ptr(light.color));
	glUniform1f(uniform_directionalLight_int, light.intensity);
}

void LightManager::uniformSpotLight(SpotLight & light)
{
	glUniform3fv(uniform_spotLight_dir, 1, glm::value_ptr(light.direction));
	glUniform3fv(uniform_spotLight_col, 1, glm::value_ptr(light.color));
	glUniform1f(uniform_spotLight_int, light.intensity);
	glUniform3fv(uniform_spotLight_pos, 1, glm::value_ptr(light.position));
	glUniform1f(uniform_spotLight_angle, light.angle);
}

#include "Lights.h"

Light::Light(float _intensity, glm::vec3 _color) : intensity(_intensity), color(_color)
{

}

///////////////////////////////

PointLight::PointLight(float _intensity, glm::vec3 _color, glm::vec3 _position) : Light(_intensity, _color), position(_position)
{

}

void PointLight::drawUI(int id)
{
	if (ImGui::CollapsingHeader(("point light " + patch::to_string(id)).c_str()))
	{
		ImGui::PushID(("point" + patch::to_string(id)).c_str());
		ImGui::SliderFloat("light intensity", &intensity, 0.f, 50.f);
		ImGui::ColorEdit3("light color", &color[0]);
		ImGui::SliderFloat3("light position", &position[0], -10.f, 10.f);
		ImGui::PopID();
	}
}

////////////////////////////////

DirectionalLight::DirectionalLight(float _intensity, glm::vec3 _color, glm::vec3 _direction) :
	Light(_intensity, _color), direction(_direction)
{

}

void DirectionalLight::drawUI(int id)
{
	if (ImGui::CollapsingHeader(("directional light " + patch::to_string(id)).c_str()))
	{
		ImGui::PushID(("directional" + patch::to_string(id)).c_str());
		ImGui::SliderFloat("light intensity", &intensity, 0.f, 50.f);
		ImGui::ColorEdit3("light color", &color[0]);
		ImGui::SliderFloat3("light direction", &direction[0], 0.f, 1.f);
		ImGui::PopID();
	}
}

////////////////////////////////////

SpotLight::SpotLight(float _intensity, glm::vec3 _color, glm::vec3 _position, glm::vec3 _direction, float _angle) :
	Light(_intensity, _color), position(_position), direction(_direction), angle(_angle)
{

}

void SpotLight::drawUI(int id)
{
	if (ImGui::CollapsingHeader(("spot light " + patch::to_string(id)).c_str()))
	{
		ImGui::PushID(("spot" + patch::to_string(id)).c_str());
		ImGui::SliderFloat("light intensity", &intensity, 0.f, 50.f);
		ImGui::ColorEdit3("light color", &color[0]);
		ImGui::SliderFloat3("light position", &position[0], -10.f, 10.f);
		ImGui::SliderFloat3("light direction", &direction[0], -1.f, 1.f);
		ImGui::SliderFloat("light angles", &angle, 0.f, glm::pi<float>());
		ImGui::PopID();
	}
}
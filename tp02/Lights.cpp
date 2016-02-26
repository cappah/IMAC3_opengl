#include "Lights.h"
#include "Scene.h"

Light::Light(float _intensity, glm::vec3 _color) : Component(LIGHT), intensity(_intensity), color(_color)
{

}

Light::~Light()
{
}

float Light::getIntensity() const
{
	return intensity;
}

glm::vec3 Light::getColor() const
{
	return color;
}

void Light::setIntensity(float i)
{
	intensity = i;

	//light bounding box is based on the intensity, so we have to update it when we change the light intensity
	updateBoundingBox();
}

void Light::setColor(const glm::vec3 & c)
{
	color = c;

	//light bounding box is based on the colot, so we have to update it when we change the light color
	updateBoundingBox();
}

void Light::updateBoundingBox()
{
	//nothing by default
}

///////////////////////////////

PointLight::PointLight(float _intensity, glm::vec3 _color, glm::vec3 _position) : Light(_intensity, _color), position(_position)
{
	m_type = POINT_LIGHT;

	updateBoundingBox();
}

PointLight::~PointLight()
{
}

void PointLight::updateBoundingBox()
{
	float lightRadius = std::sqrt(intensity * (0.3*color.r + 0.6*color.g + 0.1*color.b) / 0.003f); // radius based on light intensity and light luminance
	
	boundingBox.applyTranslation(position);
	boundingBox.applyScale(glm::vec3(lightRadius, lightRadius, lightRadius));
}

void PointLight::drawUI(Scene& scene)
{
	if (ImGui::CollapsingHeader("point light"))
	{
		if (ImGui::SliderFloat("light intensity", &intensity, 0.f, 50.f))
			updateBoundingBox();
		if (ImGui::ColorEdit3("light color", &color[0]))
			updateBoundingBox();
	}
}

void PointLight::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
{
	position = translation;

	updateBoundingBox();
}

void PointLight::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}


Component* PointLight::clone(Entity* entity)
{
	PointLight* newPointLight = new PointLight(*this);

	newPointLight->attachToEntity(entity);

	return newPointLight;
}

void PointLight::addToScene(Scene& scene)
{
	scene.add(this);
}

void PointLight::addToEntity(Entity& entity)
{
	entity.add(this);
}

void PointLight::eraseFromEntity(Entity& entity)
{
	entity.erase(this);
}

void PointLight::setBoundingBoxVisual(Mesh* visualMesh, MaterialUnlit* visualMaterial)
{
	boundingBox.setVisual(visualMesh, visualMaterial);
}

void PointLight::renderBoundingBox(const glm::mat4& projectile, const glm::mat4& view, glm::vec3 color)
{
	boundingBox.render(projectile, view, color);
}


////////////////////////////////

DirectionalLight::DirectionalLight(float _intensity, glm::vec3 _color, glm::vec3 _direction) :
	Light(_intensity, _color), direction(_direction), up(1,0,0)
{
	m_type = DIRECTIONAL_LIGHT;
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::drawUI(Scene& scene)
{
	if (ImGui::CollapsingHeader("directional light"))
	{
		ImGui::SliderFloat("light intensity", &intensity, 0.f, 10.f);
		ImGui::ColorEdit3("light color", &color[0]);
	}
}

void DirectionalLight::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
{
	glm::mat3 rotMat = glm::mat3_cast(rotation);
	up = glm::normalize( rotMat * glm::vec3(1, 0, 0) );
	direction = glm::normalize( rotMat * glm::vec3(0, -1, 0) );
}

void DirectionalLight::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

Component* DirectionalLight::clone(Entity* entity)
{
	DirectionalLight* newDirectionalLight = new DirectionalLight(intensity, color, direction);

	newDirectionalLight->attachToEntity(entity);

	return newDirectionalLight;
}

void DirectionalLight::addToScene(Scene& scene)
{
	scene.add(this);
}

void DirectionalLight::addToEntity(Entity& entity)
{
	entity.add(this);
}

void DirectionalLight::eraseFromEntity(Entity& entity)
{
	entity.erase(this);
}

////////////////////////////////////

SpotLight::SpotLight(float _intensity, glm::vec3 _color, glm::vec3 _position, glm::vec3 _direction, float _angle) :
	Light(_intensity, _color), position(_position), direction(_direction), angle(_angle), up(1,0,0)
{
	m_type = SPOT_LIGHT;

	updateBoundingBox();
}

SpotLight::~SpotLight()
{
}

void SpotLight::updateBoundingBox()
{
	float lightRadius = std::sqrt(intensity * (0.3*color.r + 0.6*color.g + 0.1*color.b) / 0.003f); //radius based on light intensity and light luminance

	boundingBox.applyTranslation(position);
	boundingBox.applyScale(glm::vec3(lightRadius, lightRadius, lightRadius));
}

void SpotLight::drawUI(Scene& scene)
{
	if (ImGui::CollapsingHeader("spot light"))
	{
		if (ImGui::SliderFloat("light intensity", &intensity, 0.f, 50.f))
			updateBoundingBox();
		if (ImGui::ColorEdit3("light color", &color[0]))
			updateBoundingBox();

		ImGui::SliderFloat("light angles", &angle, 0.f, glm::pi<float>());
	}
}

void SpotLight::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
{
	position = translation;

	glm::mat3 rotMat = glm::mat3_cast(rotation);
	up = glm::normalize(rotMat * glm::vec3(1, 0, 0));
	direction = glm::normalize(rotMat * glm::vec3(0, -1, 0));

	updateBoundingBox();
}

void SpotLight::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

Component* SpotLight::clone(Entity* entity)
{
	SpotLight* newSpotLight = new SpotLight(intensity, color, position, direction, angle);

	newSpotLight->attachToEntity(entity);

	return newSpotLight;
}

void SpotLight::addToScene(Scene& scene)
{
	scene.add(this);
}

void SpotLight::addToEntity(Entity& entity)
{
	entity.add(this);
}

void SpotLight::eraseFromEntity(Entity& entity)
{
	entity.erase(this);
}

void SpotLight::setBoundingBoxVisual(Mesh* visualMesh, MaterialUnlit* visualMaterial)
{
	boundingBox.setVisual(visualMesh, visualMaterial);
}

void SpotLight::renderBoundingBox(const glm::mat4& projectile, const glm::mat4& view, glm::vec3 color)
{
	boundingBox.render(projectile, view, color);
}
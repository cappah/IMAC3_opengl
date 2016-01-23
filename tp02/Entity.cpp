#include "Entity.h"
#include "Component.h"
#include "Scene.h"

Transform::Transform() : m_translation(0,0,0), m_scale(1,1,1)
{

}

glm::mat4 Transform::getModelMatrix()
{
	return m_modelMatrix;
}

glm::vec3 Transform::getTranslation()
{
	return m_translation;
}

glm::vec3 Transform::getScale()
{
	return m_scale;
}

glm::quat Transform::getRotation()
{
	return m_rotation;
}

void Transform::translate(glm::vec3 const& t)
{
	m_translation += t;

	updateModelMatrix();
}

void Transform::setTranslation(glm::vec3 const& t)
{
	m_translation = t;

	updateModelMatrix();
}

void Transform::scale(glm::vec3 const& s)
{
	m_scale *= s;

	updateModelMatrix();
}

void Transform::setScale(glm::vec3 const& s)
{
	m_scale = s;

	updateModelMatrix();
}

void Transform::rotate(glm::quat const& q)
{
	m_rotation *= q;

	updateModelMatrix();
}

void Transform::setRotation(glm::quat const& q)
{
	m_rotation = q;

	updateModelMatrix();
}

void Transform::updateModelMatrix()
{
	m_modelMatrix = glm::translate(glm::mat4(1), m_translation) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1), m_scale);

	onChangeModelMatrix();
}

//////////////////////////////

Entity::Entity(Scene* scene) : Transform(), m_scene(scene), m_isSelected(false)
{
	m_name.reserve(100);
	m_name[0] = '\0';

	scene->add(this);
}

Entity::~Entity()
{
	for (int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->eraseFromScene(*m_scene);
	}
	m_components.clear();
}

void Entity::onChangeModelMatrix()
{
	applyTransform();
}

void Entity::applyTransform()
{
	//if (collider != nullptr)
	//	collider->applyTransform(m_translation, m_scale);

	for (auto c : m_components)
	{
		c->applyTransform(m_translation, m_scale, m_rotation);
	}
}

void Entity::drawUI()
{
	ImGui::Begin("entity");
	ImGui::InputText("name", &m_name[0], m_name.capacity());

	bool transformationChanged = false;
	glm::vec3 tmpRot = glm::eulerAngles(m_rotation);
	ImGui::SliderFloat3("rotation", &tmpRot[0], 0, 2 * glm::pi<float>());
	glm::vec3 tmpScale = m_scale;
	ImGui::InputFloat3("scale", &tmpScale[0]);

	glm::quat tmpRotQuat(tmpRot);
	float matching = glm::dot(tmpRotQuat, m_rotation);

	if (glm::abs(matching - 1.f) < 0.0001f)
	{
		m_rotation = tmpRotQuat;
		transformationChanged = true;
	}

	if (glm::distance(tmpScale, m_scale) < 0.001f)
	{
		m_scale = tmpScale;
		transformationChanged = true;
	}

	if (transformationChanged)
		applyTransform();

	for (auto c : m_components)
	{
		c->drawUI();
	}

	//if(collider != nullptr)
	//	if (ImGui::CollapsingHeader("collider"))
	//	{
	//		collider->drawUI();
	//	}

	//if (meshRenderer != nullptr)
	//	if (ImGui::CollapsingHeader("mesh renderer"))
	//	{
	//		meshRenderer->drawUI();
	//	}


	ImGui::End();
}

bool Entity::getIsSelected() const
{
	return m_isSelected;
}

std::string Entity::getName() const
{
	return m_name;
}

void Entity::select()
{
	m_isSelected = true;
}

void Entity::deselect()
{
	m_isSelected = false;
}

Entity& Entity::add(PointLight* pointLight)
{
	pointLight->attachToEntity(this);

	m_scene->add(pointLight);
	m_components.push_back(pointLight);

	return *this;
}

Entity& Entity::add(DirectionalLight* directionalLight)
{
	directionalLight->attachToEntity(this);

	m_scene->add(directionalLight);
	m_components.push_back(directionalLight);

	return *this;
}

Entity& Entity::add(SpotLight* spotLight)
{
	spotLight->attachToEntity(this);

	m_scene->add(spotLight);
	m_components.push_back(spotLight);

	return *this;
}

Entity& Entity::add(Collider * collider)
{
	collider->attachToEntity(this);

	m_scene->add(collider);
	m_components.push_back(collider);

	return *this;
}

Entity& Entity::add(MeshRenderer * meshRenderer)
{
	meshRenderer->attachToEntity(this);

	m_scene->add(meshRenderer);
	m_components.push_back(meshRenderer);

	return *this;
}

Entity& Entity::erase(PointLight * pointLight)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), pointLight);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(pointLight);
	}

	return *this;
}

Entity& Entity::erase(DirectionalLight * directionalLight)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), directionalLight);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(directionalLight);
	}

	return *this;
}

Entity& Entity::erase(SpotLight * spotLight)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), spotLight);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(spotLight);
	}

	return *this;
}

Entity& Entity::erase(Collider * collider)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), collider);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(collider);
	}

	return *this;
}

Entity& Entity::erase(MeshRenderer * meshRenderer)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), meshRenderer);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(meshRenderer);
	}

	return *this;
}

Component* Entity::getComponent(Component::ComponentType type)
{
	auto findIt = std::find_if(m_components.begin(), m_components.end(), [type](Component* c) { return c->type() == type; });

	if (findIt != m_components.end())
	{
		return *findIt;
	}

	return nullptr;
}


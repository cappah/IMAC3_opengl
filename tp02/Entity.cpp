#include "Entity.h"
#include "Component.h"
#include "Scene.h"

Transform::Transform() : m_translation(0,0,0), m_scale(1,1,1)
{

}

Transform::~Transform()
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

Entity::Entity(const Entity& other) : Transform(other), m_isSelected(other.m_isSelected), m_name(other.m_name), m_scene(other.m_scene)
{
	m_scene->add(this);

	for (int i = 0; i < other.m_components.size(); i++)
	{
		//copy the entity
		auto newComponent = other.m_components[i]->clone(this);
		//add to scene
		newComponent->addToScene(*other.m_scene); 
		//add to entity
		m_components.push_back(newComponent);
	}

	updateModelMatrix();
}

Entity& Entity::operator=(const Entity& other)
{
	Transform::operator=(other);

	m_isSelected = other.m_isSelected;
	m_name = other.m_name;
	m_scene = other.m_scene;

	m_scene->add(this);

	eraseAllComponents();
	for (int i = 0; i < other.m_components.size(); i++)
	{
		//copy the entity
		auto newComponent = other.m_components[i]->clone(this);
		//add to scene
		newComponent->addToScene(*m_scene);
		//add to entity
		m_components.push_back(newComponent);
	}

	return *this;
}

Entity::~Entity()
{
	eraseAllComponents();
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

	char tmpName[20];
	m_name.copy(tmpName, m_name.size(), 0);
	tmpName[m_name.size()] = '\0';
	if (ImGui::InputText("name", tmpName, 20))
	{
		m_name = tmpName;
	}

	if (ImGui::SliderFloat3("rotation", &m_eulerRotation[0], 0, 2 * glm::pi<float>()))
	{
		setRotation( glm::quat(m_eulerRotation) );
		applyTransform();
	}

	glm::vec3 tmpScale = m_scale;
	if (ImGui::InputFloat3("scale", &tmpScale[0]))
	{
		setScale( tmpScale );
		applyTransform();
	}

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

void Entity::eraseAllComponents()
{
	for (int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->eraseFromScene(*m_scene);
		m_components[i] = nullptr;
	}
	m_components.clear();
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


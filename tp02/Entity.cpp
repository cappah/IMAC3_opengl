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

glm::vec3 Transform::getEulerRotation()
{
	return m_eulerRotation;
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

void Transform::setEulerRotation(glm::vec3 const & q)
{
	m_eulerRotation = q;
	m_rotation = glm::quat(q);

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

	glm::vec3 tmpRot = m_eulerRotation * (180.f / glm::pi<float>());
	if (ImGui::SliderFloat3("rotation", &tmpRot[0], 0, 360 ))
	{
		m_eulerRotation = tmpRot * glm::pi<float>() / 180.f;
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

void Entity::setName(const std::string & name)
{
	m_name = name;
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

Entity & Entity::add(Physic::Flag * flag)
{
	flag->attachToEntity(this);

	m_scene->add(flag);
	m_components.push_back(flag);

	return *this;
}

Entity & Entity::add(Physic::ParticleEmitter * particleEmitter)
{
	particleEmitter->attachToEntity(this);

	m_scene->add(particleEmitter);
	m_components.push_back(particleEmitter);

	return *this;
}

Entity & Entity::add(PathPoint * pathPoint)
{
	pathPoint->attachToEntity(this);

	m_scene->add(pathPoint);
	m_components.push_back(pathPoint);

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

Entity & Entity::erase(Physic::Flag * flag)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), flag);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(flag);
	}

	return *this;
}

Entity & Entity::erase(Physic::ParticleEmitter * particleEmitter)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), particleEmitter);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(particleEmitter);
	}

	return *this;
}

Entity & Entity::erase(PathPoint * pathPoint)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), pathPoint);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(pathPoint);
	}

	return *this;
}

void Entity::endCreation()
{
	Collider* colliderComponent = static_cast<Collider*>(getComponent(Component::ComponentType::COLLIDER));
	if (colliderComponent != nullptr)
	{
		// if a component containing a mesh is present in the entity, cover it with the collider : 
		Physic::Flag* flagComponent = static_cast<Physic::Flag*>(getComponent(Component::ComponentType::FLAG));
		MeshRenderer* meshRendererComponent = static_cast<MeshRenderer*>(getComponent(Component::ComponentType::MESH_RENDERER));
		if (flagComponent != nullptr)
		{
			colliderComponent->setOrigin(flagComponent->getOrigin());
			colliderComponent->coverMesh(flagComponent->getMesh());
			colliderComponent->addOffsetScale(glm::vec3(0.f, 0.f, 0.2f));
		}
		else if (meshRendererComponent != nullptr)
		{
			colliderComponent->setOrigin(meshRendererComponent->getOrigin());
			colliderComponent->coverMesh(*meshRendererComponent->getMesh());
		}
	}
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

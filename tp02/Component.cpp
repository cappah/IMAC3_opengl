#include "Component.h"
#include "Entity.h"



Component::Component(Component::ComponentType type) : m_entity(nullptr), m_type(type)
{
}

Component::Component(const Component& other) : m_entity(nullptr), m_type(other.m_type)
{

}

Component& Component::operator=(const Component& other)
{
	m_entity = nullptr;
	m_type = other.m_type;

	return *this;
}

Component::~Component()
{
	m_entity = nullptr;
}

void Component::attachToEntity(Entity * entity)
{
	m_entity = entity;
}

Entity* Component::entity()
{
	return m_entity;
}

Component::ComponentType Component::type() const
{
	return m_type;
}

void Component::applyTransform(const glm::vec3& translation, const glm::vec3& scale, const glm::quat& rotation)
{
	//to override
}

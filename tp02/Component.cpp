#include "Component.h"
#include "Entity.h"

const std::vector<std::string> Component::ComponentTypeName = { "box collider", "capsule collider", "sphere collider", "mesh collider", "mesh renderer", "point light", "directional light", "spot light", "flag", "particle emitter", "path point", "billboard", "camera", "wind zone", "rigidbody", "animator", "character controller", "componentCount", "behavior", "light", "none" };


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

void Component::applyTransformFromPhysicSimulation(const glm::vec3 & translation, const glm::quat & rotation)
{
	//to override
}

void Component::save(Json::Value & componentRoot) const
{
	//m_entity already set
	componentRoot["type"] = (int)m_type;
}

void Component::load(Json::Value & componentRoot)
{
	//m_entity already set
	m_type = (ComponentType)componentRoot.get("type", 0).asInt();
}

#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

//forward
class Entity;
class Scene;

class Component
{
public:
	enum ComponentType {COLLIDER, MESH_RENDERER, LIGHT, POINT_LIGHT, DIRECTIONAL_LIGHT, SPOT_LIGHT, COMPONENT_COUNT};

protected:
	Entity* m_entity;
	ComponentType m_type;

public:
	Component(ComponentType type);
	//copying component break the link to the entity, you have to call attachToEntity to be sure to attach the new component to the good entity
	Component(const Component& other);
	//copying component break the link to the entity, you have to call attachToEntity to be sure to attach the new component to the good entity
	Component& operator=(const Component& other);
	virtual ~Component();

	void attachToEntity(Entity* entity);
	
	Entity* entity();
	ComponentType type() const;

	virtual void drawUI() = 0;

	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1,1,1), const glm::quat& rotation = glm::quat());

	virtual void eraseFromScene(Scene& scene) = 0;
	virtual void addToScene(Scene& scene) = 0;

	//clone a component, and attach it to the given entity
	virtual Component* clone(Entity* entity) = 0;
};


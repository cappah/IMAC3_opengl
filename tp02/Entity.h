#pragma once

#include <string>
#include <algorithm>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "MeshRenderer.h"
#include "Collider.h"
#include "Lights.h"
#include "Flag.h"
#include "ParticleEmitter.h"

#include "glm/gtc/quaternion.hpp"

//forward
class Component;
class Scene;

class Transform
{
protected :
	glm::vec3 m_translation;
	glm::vec3 m_scale;
	glm::quat m_rotation;
	glm::vec3 m_eulerRotation;

	glm::mat4 m_modelMatrix;
public :
	Transform();
	virtual ~Transform();

	glm::mat4 getModelMatrix();

	glm::vec3 getTranslation();
	glm::vec3 getScale();
	glm::quat getRotation();
	glm::vec3 getEulerRotation();

	void translate(glm::vec3 const& t);
	void setTranslation(glm::vec3 const& t);
	void scale(glm::vec3 const& s);
	void setScale(glm::vec3 const& s);
	void rotate(glm::quat const& q);
	void setRotation(glm::quat const& q);
	void setEulerRotation(glm::vec3 const& q);

	void updateModelMatrix();

	virtual void onChangeModelMatrix() = 0;

};

class Entity : public Transform
{
private:

	Scene* m_scene;

	std::string m_name;

	std::vector<Component*> m_components;

	//for editing : 
	bool m_isSelected;

public:
	Entity(Scene* scene);
	Entity(const Entity& other);
	Entity& operator=(const Entity& other);
	virtual ~Entity();

	//this function is called each time the model matrix of this entity changes.It internally call apply transform. 
	virtual void onChangeModelMatrix() override;

	//apply transform on this entity, and apply transform on all its components.
	void applyTransform();

	//draw the entity UI
	void drawUI();

	//return the value of m_isSelected
	bool getIsSelected() const ;

	//get the name of this entity
	std::string getName() const;
	//set the name of this entity
	void setName(const std::string& name);

	//select this entity, set m_isSelected to true
	void select();
	//deselect this entity, set m_isSelected to false
	void deselect();

	// function to add a component. 
	Entity& add(PointLight* pointLight);
	// function to add a component. 
	Entity& add(DirectionalLight* directionalLight);
	// function to add a component. 
	Entity& add(SpotLight* spotLight);
	// function to add a component. 
	Entity& add(Collider* collider);
	// function to add a component. 
	Entity& add(MeshRenderer* meshRenderer);
	// function to add a component. 
	Entity& add(Physic::Flag* flag);
	// function to add a component. 
	Entity& add(Physic::ParticleEmitter* particleEmitter);

	// function to erase a component.
	Entity& erase(PointLight* pointLight);
	// function to erase a component.
	Entity& erase(DirectionalLight* directionalLight);
	// function to erase a component.
	Entity& erase(SpotLight* spotLight);
	// function to erase a component.
	Entity& erase(Collider* collider);
	// function to erase a component.
	Entity& erase(MeshRenderer* meshRenderer);
	// function to erase a component.
	Entity& erase(Physic::Flag* flag);
	// function to erase a component.
	Entity& erase(Physic::ParticleEmitter* particleEmitter);

	//finalyze the creation of the entity, should be called after all components has been added to the entity : 
	//One of the goal of this function is to properly set up the collider such that it cover well all the components of the entity.
	void endCreation();

	//delete all the components attached to this entity.
	void eraseAllComponents();

	// function to get component.
	Component* getComponent(Component::ComponentType type);

};

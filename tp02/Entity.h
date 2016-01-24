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

	glm::mat4 m_modelMatrix;
public :
	Transform();

	glm::mat4 getModelMatrix();

	glm::vec3 getTranslation();
	glm::vec3 getScale();
	glm::quat getRotation();

	void translate(glm::vec3 const& t);
	void setTranslation(glm::vec3 const& t);
	void scale(glm::vec3 const& s);
	void setScale(glm::vec3 const& s);
	void rotate(glm::quat const& q);
	void setRotation(glm::quat const& q);

	void updateModelMatrix();

	virtual void onChangeModelMatrix() = 0;

};

class Entity : public Transform
{
private:

	Scene* m_scene;

	std::string m_name;

	//MeshRenderer* meshRenderer;
	//Collider* collider;

	std::vector<Component*> m_components;

	bool m_isSelected;

public:
	Entity(Scene* scene);
	Entity(const Entity& other);
	Entity& operator=(const Entity& other);
	~Entity();

	virtual void onChangeModelMatrix() override;

	void applyTransform();

	void drawUI();

	bool getIsSelected() const ;
	std::string getName() const;

	void select();
	void deselect();

	// functions to add components : 
	Entity& add(PointLight* pointLight);
	Entity& add(DirectionalLight* directionalLight);
	Entity& add(SpotLight* spotLight);
	Entity& add(Collider* collider);
	Entity& add(MeshRenderer* meshRenderer);

	// functions to erase components : 
	Entity& erase(PointLight* pointLight);
	Entity& erase(DirectionalLight* directionalLight);
	Entity& erase(SpotLight* spotLight);
	Entity& erase(Collider* collider);
	Entity& erase(MeshRenderer* meshRenderer);

	void eraseAllComponents();

	// function to get component : 
	Component* getComponent(Component::ComponentType type);

};

#include "Entity.h"

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

Entity::Entity() : Transform(), meshRenderer(nullptr), collider(nullptr)
{

}

void Entity::onChangeModelMatrix()
{
	applyTransform();
}

void Entity::applyTransform()
{
	if (collider != nullptr)
		collider->applyTransform(m_translation, m_scale);
}

void Entity::drawUI()
{
	ImGui::Begin("entity");
	ImGui::InputText(name.c_str(), &name[0], name.size());
	ImGui::End();
	//...TODO
}
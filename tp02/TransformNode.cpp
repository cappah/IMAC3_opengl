#include "TransformNode.h"


TransformNode::TransformNode() : m_translation(0, 0, 0), m_scale(1, 1, 1)
{

}

TransformNode::~TransformNode()
{

}

glm::mat4 TransformNode::getModelMatrix()
{
	return m_modelMatrix;
}

glm::vec3 TransformNode::getTranslation()
{
	return m_translation;
}

glm::vec3 TransformNode::getScale()
{
	return m_scale;
}

glm::quat TransformNode::getRotation()
{
	return m_rotation;
}

glm::vec3 TransformNode::getEulerRotation()
{
	return m_eulerRotation;
}

void TransformNode::translate(glm::vec3 const& t)
{
	m_translation += t;

	updateModelMatrix();
}

void TransformNode::setTranslation(glm::vec3 const& t)
{
	m_translation = t;

	updateModelMatrix();
}

void TransformNode::scale(glm::vec3 const& s)
{
	m_scale *= s;

	updateModelMatrix();
}

void TransformNode::setScale(glm::vec3 const& s)
{
	m_scale = s;

	updateModelMatrix();
}

void TransformNode::rotate(glm::quat const& q)
{
	m_rotation *= q;

	updateModelMatrix();
}

void TransformNode::setRotation(glm::quat const& q)
{
	m_rotation = q;

	updateModelMatrix();
}

void TransformNode::setEulerRotation(glm::vec3 const & q)
{
	m_eulerRotation = q;
	setRotation(glm::quat(m_eulerRotation));

	updateModelMatrix();
}

void TransformNode::updateModelMatrix()
{
	m_modelMatrix = glm::translate(glm::mat4(1), m_translation) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1), m_scale);

	onChangeModelMatrix();
}

void TransformNode::drawUI()
{
	glm::vec3 tmpRot = m_eulerRotation * (180.f / glm::pi<float>());
	if (ImGui::SliderFloat3("rotation", &tmpRot[0], 0, 360))
	{
		//m_eulerRotation = tmpRot * glm::pi<float>() / 180.f;
		//setRotation(glm::quat(m_eulerRotation));
		setEulerRotation(tmpRot * glm::pi<float>() / 180.f);
		applyTransform();
	}

	glm::vec3 tmpScale = m_scale;
	if (ImGui::InputFloat3("scale", &tmpScale[0]))
	{
		setScale(tmpScale);
		applyTransform();
	}
}

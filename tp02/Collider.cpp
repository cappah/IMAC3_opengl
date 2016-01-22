#include "Collider.h"

Collider::Collider(MeshRenderer* _visual) : visual(_visual)
{
	localTopRight = glm::vec3(0.5f, 0.5f, 0.5f);
	localBottomLeft = glm::vec3(-0.5f, -0.5f, -0.5f);

	topRight = localTopRight;
	bottomLeft = localBottomLeft;
}

void Collider::setVisual(MeshRenderer* _visual)
{
	visual = _visual;
}

void Collider::applyTransform(const glm::vec3& translation, const glm::vec3& scale)
{
	topRight = localTopRight /* scale */+ translation;
	bottomLeft = localBottomLeft /* scale*/ + translation;
}

//keep the translation, apply only the scale
void Collider::applyScale(const glm::vec3& scale)
{
	topRight = localTopRight * scale;
	bottomLeft = localBottomLeft * scale;
}

//keep the translation, append the scale 
void Collider::appendScale(const glm::vec3& scale)
{
	topRight = topRight * scale;
	bottomLeft = bottomLeft * scale;
}

//keep the scale, apply only the translation
void Collider::applyTranslation(const glm::vec3& translation)
{
	topRight = localTopRight + translation;
	bottomLeft = localBottomLeft + translation;
}

//keep the scale, append the translation 
void Collider::appendTranslation(const glm::vec3& translation)
{
	topRight = topRight + translation;
	bottomLeft = bottomLeft + translation;
}

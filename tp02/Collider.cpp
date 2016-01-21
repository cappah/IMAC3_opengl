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

void Collider::applyTransform(const glm::mat4& transformMatrix)
{
	glm::vec3 translateVector(transformMatrix[3][0], transformMatrix[3][1], transformMatrix[3][2]);
	glm::vec3 scaleVector(transformMatrix[0][0], transformMatrix[1][1], transformMatrix[2][2]);

	topRight = localTopRight * scaleVector + translateVector;
	bottomLeft = localBottomLeft * scaleVector + translateVector;
}

//keep the translation, apply only the scale
void Collider::applyScale(const glm::mat4& transformMatrix)
{
	glm::vec3 scaleVector(transformMatrix[0][0], transformMatrix[1][1], transformMatrix[2][2]);

	topRight = localTopRight * scaleVector;
	bottomLeft = localBottomLeft * scaleVector;
}

//keep the translation, append the scale 
void Collider::appendScale(const glm::mat4& transformMatrix)
{
	glm::vec3 scaleVector(transformMatrix[0][0], transformMatrix[1][1], transformMatrix[2][2]);

	topRight = topRight * scaleVector;
	bottomLeft = bottomLeft * scaleVector;
}

//keep the scale, apply only the translation
void Collider::applyTranslation(const glm::mat4& transformMatrix)
{
	glm::vec3 translateVector(transformMatrix[3][0], transformMatrix[3][1], transformMatrix[3][2]);

	topRight = localTopRight + translateVector;
	bottomLeft = localBottomLeft + translateVector;
}

//keep the scale, append the translation 
void Collider::appendTranslation(const glm::mat4& transformMatrix)
{
	glm::vec3 translateVector(transformMatrix[3][0], transformMatrix[3][1], transformMatrix[3][2]);

	topRight = topRight + translateVector;
	bottomLeft = bottomLeft + translateVector;
}

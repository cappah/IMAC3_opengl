#include "Collider.h"
#include "Ray.h"
#include "Scene.h"

Collider::Collider(MeshRenderer* _visual) : Component(COLLIDER), visual(_visual)
{

}

Collider::~Collider()
{

}

void Collider::setVisual(MeshRenderer* _visual)
{
	visual = _visual;
}

void Collider::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
{
	applyTransform(translation, scale);
}

void Collider::applyTransform(const glm::vec3& _translation, const glm::vec3& _scale)
{
	translation = _translation;
	scale = _scale;

	updateModelMatrix();
}

//keep the translation, apply only the scale
void Collider::applyScale(const glm::vec3& _scale)
{
	scale = _scale;

	updateModelMatrix();
}

//keep the translation, append the scale 
void Collider::appendScale(const glm::vec3& _scale)
{
	scale *= _scale;

	updateModelMatrix();
}

//keep the scale, apply only the translation
void Collider::applyTranslation(const glm::vec3& _translation)
{
	translation = _translation;

	updateModelMatrix();
}

//keep the scale, append the translation 
void Collider::appendTranslation(const glm::vec3& _translation)
{
	translation += _translation;

	updateModelMatrix();
}

void Collider::setOffset(glm::vec3 _offset)
{
	offset = _offset;

	updateModelMatrix();
}

glm::mat4 Collider::getModelMatrix()
{
	return modelMatrix;
}

void Collider::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

void Collider::drawUI()
{
	glm::vec3 tmpOffset = offset;
	if (ImGui::InputFloat3("offset", &tmpOffset[0]))
		setOffset(tmpOffset);
}


///////////////////////////////////////////

BoxCollider::BoxCollider(MeshRenderer* _visual) : Collider(_visual)
{
	localTopRight = glm::vec3(0.5f, 0.5f, 0.5f);
	localBottomLeft = glm::vec3(-0.5f, -0.5f, -0.5f);

	topRight = localTopRight;
	bottomLeft = localBottomLeft;
}

void BoxCollider::updateModelMatrix()
{
	modelMatrix = glm::translate(glm::mat4(1), offset + translation) * glm::scale(glm::mat4(1), scale);

	topRight = glm::vec3( modelMatrix * glm::vec4(localTopRight, 1) );
	bottomLeft = glm::vec3( modelMatrix * glm::vec4(localBottomLeft, 1) );
}

void BoxCollider::render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& color)
{
	if (visual == nullptr)
		return;

	glm::mat4 mvp = projection * view * modelMatrix;

	MaterialUnlit* unlitMat = static_cast<MaterialUnlit*>(visual->material);

	unlitMat->use();
	unlitMat->setUniform_MVP(mvp);
	unlitMat->setUniform_normalMatrix(glm::mat4(1)); //no need normals
	unlitMat->setUniform_color(color);

	visual->mesh->draw();
}

void BoxCollider::debugLog()
{
	std::cout << "collider bottomLeft : " << bottomLeft.x << ", " << bottomLeft.y << ", " << bottomLeft.z << std::endl;
	std::cout << "collider topRight : " << topRight.x << ", " << topRight.y << ", " << topRight.z << std::endl;
}

bool BoxCollider::isIntersectedByRay(const Ray& ray, float* t)
{
	glm::vec3 dir = ray.getDirection();
	glm::vec3 orig = ray.getOrigin();

	// r.dir is unit direction vector of ray
	glm::vec3 dirfrac;
	dirfrac.x = 1.0f / dir.x;
	dirfrac.y = 1.0f / dir.y;
	dirfrac.z = 1.0f / dir.z;
	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray
	float t1 = (bottomLeft.x - orig.x)*dirfrac.x;
	float t2 = (topRight.x - orig.x)*dirfrac.x;
	float t3 = (bottomLeft.y - orig.y)*dirfrac.y;
	float t4 = (topRight.y - orig.y)*dirfrac.y;
	float t5 = (bottomLeft.z - orig.z)*dirfrac.z;
	float t6 = (topRight.z - orig.z)*dirfrac.z;

	float tmin = glm::max<float>(glm::max<float>(glm::min<float>(t1, t2), glm::min<float>(t3, t4)), glm::min<float>(t5, t6));
	float tmax = glm::min<float>(glm::min<float>(glm::max<float>(t1, t2), glm::max<float>(t3, t4)), glm::max<float>(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0)
	{
		if(t!=nullptr)
			*t = tmax;

		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		if (t != nullptr)
			*t = tmax;

		return false;
	}

	if (t != nullptr)
		*t = tmin;

	if (tmin > ray.getLength())
		return false;

	return true;

	/*


	glm::vec3 t;
	int maxIndex = 0;
	for (int i = 0; i < 3; i++)
	{
	if (direction[i] > 0)
	t[i] = (other.bottomLeft[i] - origin[i]) / direction[i];
	else
	t[i] = (other.topRight[i] - origin[i]) / direction[i];

	if (t[i] > t[maxIndex])
	{
	maxIndex = i;
	}
	}

	if (t[maxIndex] >= 0 && t[maxIndex] < length)
	{
	glm::vec3 pt = at(t[maxIndex]);

	int o1 = (maxIndex + 1) % 3;
	int o2 = (maxIndex + 2) % 3;

	return((pt[o1] > other.bottomLeft[o1] && pt[o1] < other.topRight[o1]) &&
	(pt[o2] > other.bottomLeft[o2] && pt[o2] < other.topRight[o2]));
	}

	return false;*/
}

void BoxCollider::drawUI()
{
	if (ImGui::CollapsingHeader("collider"))
	{
		Collider::drawUI();
	}
}

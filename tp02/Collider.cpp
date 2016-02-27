#include "Collider.h"
#include "Ray.h"
#include "Scene.h"
#include "Entity.h"

Collider::Collider(Mesh* _visualMesh, MaterialUnlit* _visualMaterial) : Component(COLLIDER), visualMesh(_visualMesh), visualMaterial(_visualMaterial), translation(0,0,0), scale(1,1,1), offsetPosition(0,0,0), offsetScale(1,1,1), origin(0,0,0)
{

}

Collider::~Collider()
{

}

void Collider::setVisual(Mesh* _visualMesh, MaterialUnlit* _visualMaterial)
{
	visualMesh = _visualMesh;
	visualMaterial = _visualMaterial;
}

void Collider::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
{
	this->rotation = rotation;

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

void Collider::applyRotation(const glm::quat & _rotation)
{
	rotation = _rotation;

	updateModelMatrix();
}

void Collider::appendRotation(const glm::quat & _rotation)
{
	rotation *= _rotation;

	updateModelMatrix();
}

void Collider::setOffsetPosition(glm::vec3 _offset)
{
	offsetPosition = _offset;

	updateModelMatrix();
}

void Collider::setOffsetScale(glm::vec3 _offset)
{
	offsetScale = _offset;

	updateModelMatrix();
}

void Collider::addOffsetPosition(glm::vec3 _offset)
{
	offsetPosition += _offset;

	updateModelMatrix();
}

void Collider::addOffsetScale(glm::vec3 _offset)
{
	offsetScale += _offset;

	updateModelMatrix();
}

void Collider::setOrigin(const glm::vec3 & _origin)
{
	origin = _origin;

	updateModelMatrix();
}

glm::vec3 Collider::getOrigin() const
{
	return origin;
}

glm::mat4 Collider::getModelMatrix()
{
	return modelMatrix;
}

void Collider::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

void Collider::drawUI(Scene& scene)
{
	glm::vec3 tmpOffset = offsetPosition;
	if (ImGui::InputFloat3("offset position", &tmpOffset[0]))
		setOffsetPosition(tmpOffset);
	glm::vec3 tmpOffsetScale = offsetScale;
	if (ImGui::InputFloat3("offset scale", &tmpOffsetScale[0]))
		setOffsetScale(tmpOffsetScale);
}


///////////////////////////////////////////

BoxCollider::BoxCollider(Mesh* _visualMesh, MaterialUnlit* _visualMaterial): Collider(_visualMesh, _visualMaterial)
{
	localTopRight = glm::vec3(0.5f, 0.5f, 0.5f);
	localBottomLeft = glm::vec3(-0.5f, -0.5f, -0.5f);

	topRight = localTopRight;
	bottomLeft = localBottomLeft;
}

void BoxCollider::updateModelMatrix()
{
	modelMatrix = glm::translate(glm::mat4(1), offsetPosition + translation) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1), scale * offsetScale) * glm::translate(glm::mat4(1), -origin);

	topRight = glm::vec3( modelMatrix * glm::vec4(localTopRight, 1) );
	bottomLeft = glm::vec3( modelMatrix * glm::vec4(localBottomLeft, 1) );
}

void BoxCollider::render(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& color)
{
	if (visualMesh == nullptr || visualMaterial == nullptr)
		return;

	glm::mat4 mvp = projection * view * modelMatrix;

	MaterialUnlit* unlitMat = static_cast<MaterialUnlit*>(visualMaterial);

	unlitMat->use();
	unlitMat->setUniform_MVP(mvp);
	unlitMat->setUniform_normalMatrix(glm::mat4(1)); //no need normals
	unlitMat->setUniform_color(color);

	visualMesh->draw();
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
	/*
	glm::vec3 H[3];
	H[0] = rotation*glm::vec3(1.f, 0.f, 0.f);
	H[1] = rotation*glm::vec3(0.f, 1.f, 0.f);
	H[2] = rotation*glm::vec3(0.f, 0.f, 1.f);
	bool test = rayOBBoxIntersect(orig, dir, translation, H, scale, t);
	return test;
	*/
	
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

void BoxCollider::drawUI(Scene& scene)
{
	Collider::drawUI(scene);
}

Component* BoxCollider::clone(Entity* entity)
{
	BoxCollider* newCollider = new BoxCollider(*this);

	newCollider->attachToEntity(entity);

	return newCollider;
}

void BoxCollider::addToScene(Scene& scene)
{
	scene.add(this);
}

void BoxCollider::addToEntity(Entity & entity)
{
	entity.add(this);
}

void BoxCollider::eraseFromEntity(Entity& entity)
{
	entity.erase(this);
}

void BoxCollider::coverMesh(Mesh& mesh)
{
	origin = mesh.origin;
	glm::vec3 dimensions = mesh.topRight - mesh.bottomLeft;

	offsetScale = dimensions;
	offsetPosition = dimensions * 0.5f + origin*dimensions + mesh.bottomLeft;

	updateModelMatrix();
}

void BoxCollider::cover(glm::vec3 min, glm::vec3 max, glm::vec3 origin)
{
	origin = origin;
	glm::vec3 dimensions = min - max;

	offsetScale = dimensions;
	offsetPosition = dimensions * 0.5f + origin*dimensions + min;

	updateModelMatrix();
}


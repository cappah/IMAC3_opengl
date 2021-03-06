#include "Collider.h"
//forwards : 
#include "Ray.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"
#include "Rigidbody.h"

CollisionInfo::CollisionInfo(Rigidbody* _rigidbody, const glm::vec3& _point, const glm::vec3& _normal) : rigidbody(_rigidbody), point(_point), normal(_normal)
{ }

///////////////////////////////

Collider::Collider(ComponentType colliderType, Mesh* _visualMesh, MaterialUnlit* _visualMaterial) : Component(colliderType), visualMesh(_visualMesh), visualMaterial(_visualMaterial), translation(0,0,0), scale(1,1,1), offsetPosition(0,0,0), offsetScale(1,1,1), origin(0,0,0)
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

void Collider::applyTransformFromPhysicSimulation(const glm::vec3 & translation, const glm::quat & rotation)
{
	this->rotation = rotation;
	this->translation = translation;

	updateModelMatrix();
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
	updateOffsetMatrix();
	updateModelMatrix();
}

void Collider::setOffsetScale(glm::vec3 _offset)
{
	offsetScale = _offset;
	updateOffsetMatrix();
	updateModelMatrix();
}

void Collider::addOffsetPosition(glm::vec3 _offset)
{
	offsetPosition += _offset;
	updateOffsetMatrix();
	updateModelMatrix();
}

void Collider::addOffsetScale(glm::vec3 _offset)
{
	offsetScale += _offset;
	updateOffsetMatrix();
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

glm::mat4 Collider::getOffsetMatrix() const
{
	return offsetMatrix;
}

void Collider::updateModelMatrix()
{
	modelMatrix = glm::translate(glm::mat4(1), translation) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1), scale) * offsetMatrix;
}

void Collider::updateOffsetMatrix()
{
	offsetMatrix = glm::translate(glm::mat4(1), offsetPosition) * glm::scale(glm::mat4(1), offsetScale);
	// TODO : Speed up this process
	Rigidbody* rigidbody = getComponent<Rigidbody>(Component::ComponentType::RIGIDBODY);
	if (rigidbody != nullptr) {
		rigidbody->makeShape();
	}
}

void Collider::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

void Collider::save(Json::Value & rootComponent) const
{
	Component::save(rootComponent);
 
	rootComponent["visualMaterialName"] = visualMaterial == nullptr ? "" : visualMaterial->name;
	rootComponent["visualMeshName"] =  visualMesh == nullptr ? "" : visualMesh->name;

	rootComponent["offsetPosition"] = toJsonValue(offsetPosition);
	rootComponent["offsetScale"] = toJsonValue(offsetScale);
	rootComponent["origin"] = toJsonValue(origin);
	rootComponent["translation"] = toJsonValue(translation);
	rootComponent["scale"] = toJsonValue(scale);
	rootComponent["rotation"] = toJsonValue(rotation);
	rootComponent["modelMatrix"] = toJsonValue(modelMatrix);
}

void Collider::load(Json::Value & rootComponent)
{
	Component::load(rootComponent);

	std::string visualMaterialName = rootComponent.get("visualMaterialName", "").asString();
	if (visualMaterialName != "")
		visualMaterial = MaterialFactory::get().get<MaterialUnlit>(visualMaterialName);

	std::string visualMeshName = rootComponent.get("visualMeshName", "").asString();
	if (visualMeshName != "")
		visualMesh = MeshFactory::get().get(visualMeshName);


	offsetPosition = fromJsonValue<glm::vec3>(rootComponent["offsetPosition"], glm::vec3());
	offsetScale = fromJsonValue<glm::vec3>(rootComponent["offsetScale"], glm::vec3());
	origin = fromJsonValue<glm::vec3>(rootComponent["origin"], glm::vec3());
	translation = fromJsonValue<glm::vec3>(rootComponent["translation"], glm::vec3());
	scale = fromJsonValue<glm::vec3>(rootComponent["scale"], glm::vec3());
	rotation = fromJsonValue<glm::quat>(rootComponent["rotation"], glm::quat());
	modelMatrix = fromJsonValue<glm::mat4>(rootComponent["modelMatrix"], glm::mat4());
}

void Collider::drawUI(Scene& scene)
{
	glm::vec3 tmpOffset = offsetPosition;
	if (ImGui::InputFloat3("offset position", &tmpOffset[0]))
		setOffsetPosition(tmpOffset);
}


///////////////////////////////////////////

BoxCollider::BoxCollider(Mesh* _visualMesh, MaterialUnlit* _visualMaterial): Collider(BOX_COLLIDER, _visualMesh, _visualMaterial)
{
	localTopRight = glm::vec3(0.5f, 0.5f, 0.5f);
	localBottomLeft = glm::vec3(-0.5f, -0.5f, -0.5f);

	topRight = localTopRight;
	bottomLeft = localBottomLeft;
}

void BoxCollider::updateModelMatrix()
{
	Collider::updateModelMatrix();
	//modelMatrix = glm::translate(glm::mat4(1), offsetPosition + translation) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1), scale * offsetScale) * glm::translate(glm::mat4(1), -origin);

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
	glm::vec3 tmpOffsetScale = offsetScale;
	if (ImGui::InputFloat3("offset scale", &tmpOffsetScale[0]))
		setOffsetScale(tmpOffsetScale);
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
	glm::vec3 dimensions = (mesh.topRight - mesh.bottomLeft)*scale;

	offsetScale = dimensions;
	offsetPosition = dimensions * 0.5f + origin*dimensions + mesh.bottomLeft*scale;// -translation;

	updateOffsetMatrix();
	updateModelMatrix();
}

void BoxCollider::cover(glm::vec3 min, glm::vec3 max, glm::vec3 _origin)
{
	origin = _origin;
	glm::vec3 dimensions = (min - max);

	offsetScale = dimensions;
	offsetPosition = dimensions * 0.5f + origin*dimensions + min;// -translation;

	updateOffsetMatrix();
	updateModelMatrix();
}

btCollisionShape * BoxCollider::makeShape()
{
	return new btBoxShape(btVector3(0.5f*offsetScale.x, 0.5f*offsetScale.y, 0.5f*offsetScale.z));
}

void BoxCollider::save(Json::Value & rootComponent) const
{
	Collider::save(rootComponent);

	rootComponent["localTopRight"] = toJsonValue(localTopRight);
	rootComponent["localBottomLeft"] = toJsonValue(localBottomLeft);
	rootComponent["topRight"] = toJsonValue(topRight);
	rootComponent["bottomLeft"] = toJsonValue(bottomLeft);
}

void BoxCollider::load(Json::Value & rootComponent)
{
	Collider::load(rootComponent);

	localTopRight = fromJsonValue<glm::vec3>(rootComponent["localTopRight"], glm::vec3());
	localBottomLeft = fromJsonValue<glm::vec3>(rootComponent["localBottomLeft"], glm::vec3());
	topRight = fromJsonValue<glm::vec3>(rootComponent["topRight"], glm::vec3());
	bottomLeft = fromJsonValue<glm::vec3>(rootComponent["bottomLeft"], glm::vec3());
}

//////////////////////////////////////////////

CapsuleCollider::CapsuleCollider(): Collider(CAPSULE_COLLIDER, MeshFactory::get().get("capsuleWireframe"), MaterialFactory::get().get<MaterialUnlit>("wireframe")),
									radius(0.5f), height(2.f)
{

}

void CapsuleCollider::render(const glm::mat4 & projection, const glm::mat4 & view, const glm::vec3 & color)
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

void CapsuleCollider::debugLog()
{
	//nothing
}

bool CapsuleCollider::isIntersectedByRay(const Ray & ray, float * t)
{
	//nothing
	return false;
}

void CapsuleCollider::drawUI(Scene & scene)
{
	Collider::drawUI(scene);
	if (ImGui::InputFloat("height", &height)) {
		offsetScale.y = height*0.5f;
		offsetScale.x = radius * 2.f;
		offsetScale.z = radius * 2.f;
		updateOffsetMatrix();
	}
	if (ImGui::InputFloat("radius", &radius)) {
		offsetScale.y = height*0.5f;
		offsetScale.x = radius * 2.f;
		offsetScale.z = radius * 2.f;
		updateOffsetMatrix();
	}
}

Component* CapsuleCollider::clone(Entity* entity)
{
	CapsuleCollider* newCollider = new CapsuleCollider(*this);

	newCollider->attachToEntity(entity);

	return newCollider;
}

void CapsuleCollider::addToScene(Scene& scene)
{
	scene.add(this);
}

void CapsuleCollider::addToEntity(Entity & entity)
{
	entity.add(this);
}

void CapsuleCollider::eraseFromEntity(Entity& entity)
{
	entity.erase(this);
}

void CapsuleCollider::coverMesh(Mesh & mesh)
{
	origin = mesh.origin;
	glm::vec3 dimensions = (mesh.topRight - mesh.bottomLeft)*scale;

	offsetScale = dimensions;
	offsetPosition = dimensions * 0.5f + origin*dimensions + mesh.bottomLeft*scale;// -translation;

	updateOffsetMatrix();
	updateModelMatrix();
}

void CapsuleCollider::cover(glm::vec3 min, glm::vec3 max, glm::vec3 _origin)
{
	origin = _origin;
	glm::vec3 dimensions = (min - max)*scale;

	offsetScale = dimensions;
	offsetPosition = dimensions * 0.5f + origin*dimensions + min*scale;// -translation;

	height = offsetScale.y * 2.f;
	radius = offsetScale.x*0.5f;

	updateOffsetMatrix();
	updateModelMatrix();
}

btCollisionShape * CapsuleCollider::makeShape()
{
	return new btCapsuleShape(0.5f*offsetScale.x, 2.f*offsetScale.y);
}

void CapsuleCollider::save(Json::Value & rootComponent) const
{
	Collider::save(rootComponent);
	rootComponent["height"] = height;
	rootComponent["radius"] = radius;
}

void CapsuleCollider::load(Json::Value & rootComponent)
{
	Collider::load(rootComponent);
	height = rootComponent.get("height", 1).asFloat();
	radius = rootComponent.get("radius", 1).asFloat();
}

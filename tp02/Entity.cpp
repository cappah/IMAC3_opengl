#include "Entity.h"
//forwards : 
#include "Component.h"
#include "Scene.h"
#include "ComponentFactory.h"
#include "BehaviorFactory.h"
#include "PhysicManager.h"

Entity::Entity(Scene* scene) : TransformNode(), m_scene(scene), m_isSelected(false), m_name("default_entity"), m_parent(nullptr)
{
	scene->add(this);
}

Entity::Entity(const Entity& other) : TransformNode(other), m_isSelected(other.m_isSelected), m_name(other.m_name), m_scene(other.m_scene), m_parent(other.m_parent)
{
	m_scene->add(this);

	for (int i = 0; i < other.m_components.size(); i++)
	{
		//copy the entity
		auto newComponent = other.m_components[i]->clone(this);
		//add to scene
		newComponent->addToScene(*other.m_scene); 
		//add to entity
		m_components.push_back(newComponent);
	}
	eraseAllChilds();
	for (int i = 0; i < other.m_childs.size(); i++)
	{
		auto newEntity = new Entity(*other.m_childs[i]);
		m_childs.push_back(newEntity);
	}

	updateModelMatrix();
}

Entity& Entity::operator=(const Entity& other)
{
	TransformNode::operator=(other);

	m_isSelected = other.m_isSelected;
	m_name = other.m_name;
	m_scene = other.m_scene;
	m_parent = other.m_parent;

	m_scene->add(this);

	eraseAllComponents();
	for (int i = 0; i < other.m_components.size(); i++)
	{
		//copy the entity
		auto newComponent = other.m_components[i]->clone(this);
		//add to scene
		newComponent->addToScene(*m_scene);
		//add to entity
		m_components.push_back(newComponent);
	}
	eraseAllChilds();
	for (int i = 0; i < other.m_childs.size(); i++)
	{
		auto newEntity = new Entity(*other.m_childs[i]);
		m_childs.push_back(newEntity);
	}

	return *this;
}

Entity::~Entity()
{
	setParent(nullptr);
	eraseAllComponents();
	eraseAllChilds();
}

void Entity::onChangeModelMatrix()
{
	applyTransform();
}

void Entity::applyTransform()
{
	//if (collider != nullptr)
	//	collider->applyTransform(m_translation, m_scale);

	for (auto& c : m_components)
	{
		c->applyTransform(m_translation, m_scale, m_rotation);
	}
	for (auto& e : m_childs)
	{
		e->applyTransform(m_translation, m_scale, m_rotation);
		//e->applyTransform(); //recursivity on all childs
	}
}

void Entity::applyTransform(const glm::vec3 & parentTranslation, const glm::vec3 & parentScale, const glm::quat & parentRotation)
{
	m_translation = parentRotation * m_localTranslation + parentTranslation;
	m_scale = m_localScale * parentScale;
	m_rotation = m_localRotation * parentRotation;

	//glm::mat4 updateMatrix = glm::translate(glm::mat4(1), parentTranslation);// *glm::mat4_cast(parentRotation);// *glm::scale(glm::mat4(1), parentScale);

	////combine transforms to get world model matrix of the model
	////and make the child launch applyTransform recursivly
	setParentTransform(parentTranslation, parentScale, parentRotation);
	updateModelMatrix();
}

void Entity::applyTransform(const glm::vec3 & parentTranslation, const glm::quat & parentRotation)
{
	m_translation = parentRotation * m_localTranslation + parentTranslation;
	m_rotation = m_localRotation * parentRotation;

	//glm::mat4 updateMatrix = glm::translate(glm::mat4(1), parentTranslation);// *glm::mat4_cast(parentRotation);// *glm::scale(glm::mat4(1), parentScale);

	////combine transforms to get world model matrix of the model
	////and make the child launch applyTransform recursivly
	setParentTransform(parentTranslation, parentRotation);
	updateModelMatrix();
}

void Entity::applyTransformFromPhysicSimulation(const glm::vec3 & translation, const glm::quat & rotation)
{
	//setTranslation : 
	m_translation = translation;
	m_localTranslation = translation;

	//setRotation : 
	m_rotation = rotation;
	m_localRotation = rotation;

	//updateModelMatrix : 
	m_translation = m_parentRotation * m_localTranslation + m_parentTranslation;
	m_scale = m_localScale * m_parentScale;
	m_rotation = m_localRotation * m_parentRotation;
	m_modelMatrix = glm::translate(glm::mat4(1), m_translation) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1), m_scale);

	//onChangeModelMatrix : 
	applyTransformFromPhysicSimulation();


	//m_translation = parentRotation * m_localTranslation + parentTranslation;
	//m_rotation = m_localRotation * parentRotation;

	////glm::mat4 updateMatrix = glm::translate(glm::mat4(1), parentTranslation);// *glm::mat4_cast(parentRotation);// *glm::scale(glm::mat4(1), parentScale);

	//////combine transforms to get world model matrix of the model
	//////and make the child launch applyTransform recursivly
	//setParentTransform(parentTranslation, parentRotation);
	////updateModelMatrix();
	//m_translation = m_parentRotation * m_localTranslation + m_parentTranslation;
	//m_scale = m_localScale * m_parentScale;
	//m_rotation = m_localRotation * m_parentRotation;

	//m_modelMatrix = glm::translate(glm::mat4(1), m_translation) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1), m_scale);


	//set transformations to components and childs :
	applyTransformFromPhysicSimulation();
}

void Entity::applyTransformFromPhysicSimulation()
{
	for (auto& c : m_components)
	{
		c->applyTransformFromPhysicSimulation(m_translation, m_rotation);
	}
	for (auto& e : m_childs)
	{
		e->applyTransformFromPhysicSimulation(m_translation, m_rotation);
		//e->applyTransform(); //recursivity on all childs
	}
}


void Entity::displayTreeNodeInspector(Scene& scene, Component* component, int id, bool& hasToRemoveComponent, int& removeId)
{
	ImGui::SetNextWindowContentWidth(80);
	bool nodeOpen = false;

	ImVec2 itemPos;
	ImVec2 itemSize;
	if (ImGui::MyTreeNode("", itemPos, itemSize))
		nodeOpen = true;
	ImGui::SameLine();


	ImGui::Text(Component::ComponentTypeName[component->type()].c_str());//, ImVec2(itemSize.x /*m_bottomLeftPanelRect.z*/ - 36.f, itemSize.y /*16.f*/)))

	ImGui::SameLine();
	if (ImGui::Button("remove"))
	{
		hasToRemoveComponent = true;
		removeId = id;
	}

	if (nodeOpen) {
		//ImGui::SetNextWindowContentWidth(500);
		//ImGui::BeginChild(ImGuiID(id), ImVec2(500, 0), ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_ChildWindowAutoFitY | ImGuiWindowFlags_ChildWindowAutoFitX);
		component->drawUI(scene);
		//ImGui::EndChild();
	}

	if (nodeOpen)
		ImGui::TreePop();
}

void Entity::drawUI(Scene& scene)
{

	char tmpName[20];
	m_name.copy(tmpName, m_name.size(), 0);
	tmpName[m_name.size()] = '\0';
	if (ImGui::InputText("name", tmpName, 20))
	{
		m_name = tmpName;
	}

	TransformNode::drawUI(hasParent());

	bool hasToRemoveComponent = false;
	int removeId = 0;
	bool expendComponent = false;
	for (int i = 0; i < m_components.size(); i++)
	{
		ImGui::PushID(i);
		//ImGui::Separator();
		///*const float frame_height = 10;
		//ImRect bb = ImRect(ImGui::GetCurrentWindow()->DC.CursorPos, ImVec2(ImGui::GetCurrentWindow()->Pos.x + ImGui::GetContentRegionMax().x, ImGui::GetCurrentWindow()->DC.CursorPos.y + frame_height));
		//bool hovered, held;
		//const ImGuiID id = ImGui::GetCurrentWindow()->DC. ->GetID(str_id);
		//bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_NoKeyModifiers);
		//ImGui::RenderCollapseTriangle(bb.Min, false);*/
		//if (ImGui::Button("expend"))
		//	expendComponent = true;
		//ImGui::SameLine();
		//ImGui::Text(Component::ComponentTypeName[m_components[i]->type()].c_str());
		//ImGui::SameLine();
		//if (ImGui::Button("remove"))
		//{
		//	hasToRemoveComponent = true;
		//	removeId = i;
		//}	
		//ImGui::Separator();
		//if(expendComponent)
		//	m_components[i]->drawUI(scene);
		displayTreeNodeInspector(scene, m_components[i], i, hasToRemoveComponent, removeId);

		ImGui::PopID();

		expendComponent = false;
	}
	if(hasToRemoveComponent)
		m_components[removeId]->eraseFromEntity(*this);

	if (ImGui::Button("add component"))
		ImGui::OpenPopup("add component window");

	bool addComponentWindowOpened = true;

	if (ImGui::BeginPopupModal("add component window", &addComponentWindowOpened))
	{
		ComponentFactory::get().drawModalWindow(this);
		BehaviorFactory::get().drawModalWindow(this);

		ImGui::EndPopup();
	}

}

bool Entity::getIsSelected() const
{
	return m_isSelected;
}

Entity::CollisionState Entity::getCollisionState() const
{
	return m_collisionState;
}

CollisionInfo Entity::getCollisionInfo() const
{
	return m_collisionInfo;
}

void Entity::resetCollision()
{
	m_collisionInfo.receiver = nullptr;
	m_collisionInfo.rigidbody = nullptr;

	m_collisionState = CollisionState::NONE;
}

std::string Entity::getName() const
{
	return m_name;
}

void Entity::setName(const std::string & name)
{
	m_name = name;
}

void Entity::select()
{
	m_isSelected = true;
}

void Entity::deselect()
{
	m_isSelected = false;
}

Entity& Entity::add(PointLight* pointLight)
{
	pointLight->attachToEntity(this);

	m_scene->add(pointLight);
	m_components.push_back(pointLight);

	return *this;
}

Entity& Entity::add(DirectionalLight* directionalLight)
{
	directionalLight->attachToEntity(this);

	m_scene->add(directionalLight);
	m_components.push_back(directionalLight);

	return *this;
}

Entity& Entity::add(SpotLight* spotLight)
{
	spotLight->attachToEntity(this);

	m_scene->add(spotLight);
	m_components.push_back(spotLight);

	return *this;
}

Entity& Entity::add(Collider * collider)
{
	collider->attachToEntity(this);

	m_scene->add(collider);
	m_components.push_back(collider);

	//special stuuf for colliders : 
	Rigidbody* rigidbody = getComponent<Rigidbody>(Component::ComponentType::RIGIDBODY);
	if (rigidbody != nullptr)
		rigidbody->makeShape(); //order the ridigbody to reupdate it collider shape

	return *this;
}

Entity& Entity::add(MeshRenderer * meshRenderer)
{
	meshRenderer->attachToEntity(this);

	m_scene->add(meshRenderer);
	m_components.push_back(meshRenderer);

	return *this;
}

Entity & Entity::add(Physic::Flag * flag)
{
	flag->attachToEntity(this);

	m_scene->add(flag);
	m_components.push_back(flag);

	return *this;
}

Entity & Entity::add(Physic::ParticleEmitter * particleEmitter)
{
	particleEmitter->attachToEntity(this);

	m_scene->add(particleEmitter);
	m_components.push_back(particleEmitter);

	return *this;
}

Entity & Entity::add(PathPoint * pathPoint)
{
	pathPoint->attachToEntity(this);

	m_scene->add(pathPoint);
	m_components.push_back(pathPoint);

	return *this;
}

Entity & Entity::add(Billboard * billboard)
{
	billboard->attachToEntity(this);

	m_scene->add(billboard);
	m_components.push_back(billboard);

	return *this;
}


Entity & Entity::add(Camera * camera)
{
	camera->attachToEntity(this);

	m_scene->add(camera);
	m_components.push_back(camera);

	return *this;
}

Entity & Entity::add(Physic::WindZone * windZone)
{
	windZone->attachToEntity(this);

	m_scene->add(windZone);
	m_components.push_back(windZone);

	return *this;
}

Entity & Entity::add(Rigidbody* rigidbody)
{
	rigidbody->attachToEntity(this);

	m_scene->add(rigidbody);
	m_components.push_back(rigidbody);

	//special stuff for rigidbody :
	rigidbody->makeShape(); //order the ridigbody to reupdate it collider shape
	rigidbody->init(m_scene->getPhysicManager().getBulletDynamicSimulation()); //must be call after the rigidbody has been attached to an entity

	return *this;
}

Entity & Entity::add(Animator * animator)
{
	animator->attachToEntity(this);

	m_scene->add(animator);
	m_components.push_back(animator);

	return *this;
}

Entity & Entity::add(Behavior * behavior)
{
	behavior->attachToEntity(this);

	m_scene->add(behavior);
	m_components.push_back(behavior);

	return *this;
}


Entity& Entity::erase(PointLight * pointLight)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), pointLight);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(pointLight);
	}

	return *this;
}

Entity& Entity::erase(DirectionalLight * directionalLight)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), directionalLight);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(directionalLight);
	}

	return *this;
}

Entity& Entity::erase(SpotLight * spotLight)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), spotLight);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(spotLight);
	}

	return *this;
}

Entity& Entity::erase(Collider * collider)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), collider);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(collider);
	}

	//special stuff for colliders : 
	Rigidbody* rigidbody = getComponent<Rigidbody>(Component::ComponentType::RIGIDBODY);
	if (rigidbody != nullptr)
		rigidbody->makeShape(); //order the ridigbody to reupdate it collider shape

	return *this;
}

Entity& Entity::erase(MeshRenderer * meshRenderer)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), meshRenderer);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(meshRenderer);
	}

	return *this;
}

Entity & Entity::erase(Physic::Flag * flag)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), flag);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(flag);
	}

	return *this;
}

Entity & Entity::erase(Physic::ParticleEmitter * particleEmitter)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), particleEmitter);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(particleEmitter);
	}

	return *this;
}

Entity & Entity::erase(PathPoint * pathPoint)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), pathPoint);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(pathPoint);
	}

	return *this;
}

Entity & Entity::erase(Billboard * billboard)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), billboard);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(billboard);
	}

	return *this;
}

Entity & Entity::erase(Physic::WindZone * windZone)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), windZone);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(windZone);
	}

	return *this;
}

Entity & Entity::erase(Camera * camera)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), camera);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(camera);
	}

	return *this;
}

Entity & Entity::erase(Rigidbody * rigidbody)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), rigidbody);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(rigidbody);
	}

	return *this;
}

Entity & Entity::erase(Animator * animator)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), animator);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(animator);
	}

	return *this;
}

Entity & Entity::erase(Behavior * behavior)
{
	auto findIt = std::find(m_components.begin(), m_components.end(), behavior);

	if (findIt != m_components.end())
	{
		m_components.erase(findIt);
		m_scene->erase(behavior);
	}

	return *this;
}

void Entity::endCreation()
{
	Collider* colliderComponent = static_cast<Collider*>(getComponent(Component::ComponentType::COLLIDER));
	if (colliderComponent != nullptr)
	{
		// if a component containing a mesh is present in the entity, cover it with the collider : 
		Physic::Flag* flagComponent = static_cast<Physic::Flag*>(getComponent(Component::ComponentType::FLAG));
		MeshRenderer* meshRendererComponent = static_cast<MeshRenderer*>(getComponent(Component::ComponentType::MESH_RENDERER));
		if (flagComponent != nullptr)
		{
			colliderComponent->setOrigin(flagComponent->getOrigin());
			colliderComponent->coverMesh(flagComponent->getMesh());
			colliderComponent->addOffsetScale(glm::vec3(0.f, 0.f, 0.2f));
		}
		else if (meshRendererComponent != nullptr)
		{
			colliderComponent->setOrigin(meshRendererComponent->getOrigin());
			colliderComponent->coverMesh(*meshRendererComponent->getMesh());
		}
	}
}

void Entity::eraseAllComponents()
{
	for (int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->eraseFromScene(*m_scene);
		//m_components[i] = nullptr;
	}
	m_components.clear();
}

bool Entity::hasParent() const
{
	return m_parent != nullptr;
}

bool Entity::hasChild() const
{
	return m_childs.size() > 0;
}

Entity* Entity::getChild(int idx)
{
	return m_childs[idx];
}

Entity* Entity::getParent()
{
	return m_parent;
}

void Entity::setParent(Entity* parent)
{
	if (m_parent != nullptr)
		removeParent();

	setParentAtomic(parent);
	if(m_parent != nullptr)
		m_parent->addChildAtomic(this);
}

void Entity::addChild(Entity* child)
{
	child->removeParent();

	child->setParentAtomic(this);
	addChildAtomic(child);
}

void Entity::removeParent()
{
	if(m_parent != nullptr)
		m_parent->removeChild(this);
	m_parent = nullptr;
}

void Entity::removeChild(Entity* child)
{
	if (m_childs.size() > 0) //has childs ?
	{
		auto findIt = std::find(m_childs.begin(), m_childs.end(), child);
		if (findIt != m_childs.end())
			m_childs.erase(findIt);
	}
}

void Entity::eraseAllChilds()
{
	while(m_childs.size() > 0)
	{
		m_scene->erase(m_childs.back());
		//m_childs[i] = nullptr;
	}
	if(m_childs.size() > 0)
		m_childs.clear();
}

int Entity::getChildCount() const
{
	return m_childs.size();
}

void Entity::updateCoroutines()
{
	float currentTime = Application::get().getTime();
	for (auto& c : m_coroutines) {
		if (c->getNextExecutionTime() <= currentTime)
			c->execute(currentTime);
	}
}

void Entity::onCollisionBegin(const CollisionInfo & collisionInfo)
{
	//TODO speed up this operation
	//auto behaviors = getComponents<Behavior>(Component::ComponentType::BEHAVIOR);
	//for (auto& b : behaviors) {
	//	b->onColliderEnter(collisionInfo);
	//}
	m_collisionState = CollisionState::BEGIN;
	m_collisionInfo = collisionInfo;
}

void Entity::onCollisionStay(const CollisionInfo & collisionInfo)
{
	//TODO speed up this operation
	//auto behaviors = getComponents<Behavior>(Component::ComponentType::BEHAVIOR);
	//for (auto& b : behaviors) {
	//	b->onColliderEnter(collisionInfo);
	//}
	m_collisionState = CollisionState::STAY;
	m_collisionInfo = collisionInfo;
}

void Entity::onCollisionEnd(const CollisionInfo & collisionInfo)
{
	//TODO speed up this operation
	//auto behaviors = getComponents<Behavior>(Component::ComponentType::BEHAVIOR);
	//for (auto& b : behaviors) {
	//	b->onColliderEnter(collisionInfo);
	//}
	m_collisionState = CollisionState::END;
	m_collisionInfo = collisionInfo;
}

void Entity::save(Json::Value& entityRoot) const
{
	TransformNode::save(entityRoot);

	//Scene* m_scene; scene is already set by constructor 
	entityRoot["name"] = m_name;

	//std::vector<Entity*> m_childs; TODO
	//Entity* m_parent; TODO

	//bool m_isSelected; selected information isn't serialized

	entityRoot["componentCount"] = m_components.size();
	for (int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->save(entityRoot["components"][i]);
	}

	//entityRoot["childCount"] = m_childs.size();
	//for (int i = 0; i < m_childs.size(); i++)
	//{
	//	m_childs[i]->save(entityRoot["childs"][i]);
	//}
}

void Entity::load(Json::Value& entityRoot)
{
	TransformNode::load(entityRoot);
	applyTransform();

	//Scene* m_scene; scene is already set by constructor 
	m_name = entityRoot.get("name", "defaultEntity").asString();

	//std::vector<Entity*> m_childs; TODO
	//Entity* m_parent; TODO

	//bool m_isSelected; selected information isn't serialized

	int componentCount = entityRoot.get("componentCount", 0).asInt();
	for (int i = 0; i < componentCount; i++)
	{
		Component* newComponent;
		Component::ComponentType type = (Component::ComponentType)entityRoot["components"][i].get("type", 0).asInt();
		if (type == Component::ComponentType::BEHAVIOR) {
			std::string behaviourTypeIndexName = entityRoot["components"][i].get("typeIndexName", "NONE").asString();
			if (behaviourTypeIndexName == "NONE")
				continue;
			newComponent = BehaviorFactory::get().getInstance(behaviourTypeIndexName);
		}
		else {
			newComponent = ComponentFactory::get().getInstance(type);
		}
		newComponent->load(entityRoot["components"][i]);

		newComponent->addToEntity(*this);
	}

	applyTransform();

	//int childCount = entityRoot.get("childCount", 0).asInt();
	//for (int i = 0; i < childCount; i++)
	//{
	//	Entity* newEntity = new Entity(m_scene);
	//	newEntity->load(entityRoot["childs"][i]);
	//}
}

void Entity::addChildAtomic(Entity* child)
{
	m_childs.push_back(child);
}

void Entity::setParentAtomic(Entity* parent)
{
	m_parent = parent;

	if (m_parent == nullptr)
		setParentTransform();
	else
		setParentTransform(*m_parent);
}

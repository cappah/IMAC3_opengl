#include "stdafx.h"

#include "Scene.h"
//forwards :
#include "OctreeDrawer.h"
#include "PhysicManager.h"


Scene::Scene(Renderer* renderer, const std::string& sceneName) 
	: m_renderer(renderer)
	, m_name(sceneName)
	, m_areCollidersVisible(true)
	, m_isDebugDeferredVisible(true)
	, m_isDebugPhysicVisible(true)
	, m_renderables(glm::vec3(0, 0, 0), 500, 3) //TODO RENDERING : put a flexible size
{
	m_physicManager = new Physic::PhysicManager();
	m_terrain.initPhysics(m_physicManager->getBulletDynamicSimulation());
}

Scene::~Scene()
{
	clear();
}

void Scene::clear()
{
	//Components : 
	/*
	//Cameras : 
	for (int i = 0; i < m_cameras.size(); i++)
	{
		delete m_cameras[i];
		m_cameras.clear();
	}
	//Colliders : 
	for (int i = 0; i < m_colliders.size(); i++)
	{
		delete m_colliders[i];
		m_colliders.clear();
	}
	//Directionnal lights : 
	for (int i = 0; i < m_directionalLights.size(); i++)
	{
		delete m_directionalLights[i];
		m_directionalLights.clear();
	}
	//Flags : 
	for (int i = 0; i < m_flags.size(); i++)
	{
		delete m_flags[i];
		m_flags.clear();
	}
	//Mesh renderers : 
	for (int i = 0; i < m_meshRenderers.size(); i++)
	{
		delete m_meshRenderers[i];
		m_meshRenderers.clear();
	}
	//Particle emitters : 
	for (int i = 0; i < m_particleEmitters.size(); i++)
	{
		delete m_particleEmitters[i];
		m_particleEmitters.clear();
	}
	//Point lights : 
	for (int i = 0; i < m_pointLights.size(); i++)
	{
		delete m_pointLights[i];
		m_pointLights.clear();
	}
	//Point lights : 
	for (int i = 0; i < m_spotLights.size(); i++)
	{
		delete m_spotLights[i];
		m_spotLights.clear();
	}
	//Wind zones : 
	for (int i = 0; i < m_windZones.size(); i++)
	{
		delete m_windZones[i];
		m_windZones.clear();
	}*/

	//Entities : 
	for (int i = 0; i < m_entities.size(); i++)
	{
		delete m_entities[i];
	}
	m_entities.clear();

	//clear systems : 
	m_terrain.clear();
	m_physicManager->clear();
	delete m_physicManager;
	//m_skybox.clear(); //TODO
}

std::vector<Entity*>& Scene::getEntities()
{
	return m_entities;
}

Scene& Scene::add(Entity * entity)
{
	m_entities.push_back(entity);
	return *this;
}

Scene& Scene::add(PointLight * pointLight)
{
	m_pointLights.push_back(pointLight);
	return *this;
}

Scene& Scene::add(DirectionalLight * directionalLight)
{
	m_directionalLights.push_back(directionalLight);
	return *this;
}

Scene& Scene::add(SpotLight * spotLight)
{
	m_spotLights.push_back(spotLight);
	return *this;
}

Scene& Scene::add(Collider * collider)
{
	m_colliders.push_back(collider);
	return *this;
}

Scene& Scene::add(MeshRenderer * meshRenderer)
{
	m_meshRenderers.push_back(meshRenderer);

	//Add this components to renderables :
	IRenderableComponent* asRenderable = static_cast<IRenderableComponent*>(meshRenderer);
	if(asRenderable->getDrawableCount() > 0)
		m_renderables.add(asRenderable, asRenderable->getDrawable(0).getVisualBoundingBox());

	return *this;
}

Scene& Scene::add(Physic::Flag * flag)
{
	m_flags.push_back(flag);

	//Add this components to renderables :
	IRenderableComponent* asRenderable = static_cast<IRenderableComponent*>(flag);
	if (asRenderable->getDrawableCount() > 0)
		m_renderables.add(asRenderable, asRenderable->getDrawable(0).getVisualBoundingBox());

	return *this;
}

Scene & Scene::add(Physic::ParticleEmitter * particleEmitter)
{
	m_particleEmitters.push_back(particleEmitter);

	//Add this components to renderables :
	IRenderableComponent* asRenderable = static_cast<IRenderableComponent*>(particleEmitter);
	if (asRenderable->getDrawableCount() > 0)
		m_renderables.add(asRenderable, asRenderable->getDrawable(0).getVisualBoundingBox());

	return *this;
}

Scene & Scene::add(PathPoint * pathPoint)
{
	m_pathManager.add(pathPoint);
	return *this;
}

Scene & Scene::add(Billboard * billboard)
{
	m_billboards.push_back(billboard);

	//Add this components to renderables :
	IRenderableComponent* asRenderable = static_cast<IRenderableComponent*>(billboard);
	if (asRenderable->getDrawableCount() > 0)
		m_renderables.add(asRenderable, asRenderable->getDrawable(0).getVisualBoundingBox());

	return *this;
}

Scene & Scene::add(Camera * camera)
{
	m_cameras.push_back(camera);
	return *this;
}

Scene & Scene::add(Physic::WindZone* windZone)
{
	m_windZones.push_back(windZone);
	return *this;
}

Scene & Scene::add(Rigidbody* rigidbody)
{
	m_rigidbodies.push_back(rigidbody);
	return *this;
}

Scene & Scene::add(Animator * animator)
{
	m_animators.push_back(animator);
	return *this;
}

Scene & Scene::add(CharacterController * characterController)
{
	m_characterControllers.push_back(characterController);
	return *this;
}

Scene & Scene::add(Behavior * behavior)
{
	m_behaviors.push_back(behavior);
	return *this;
}


Scene& Scene::erase(Entity* entity)
{
	auto findIt = std::find(m_entities.begin(), m_entities.end(), entity);

	if (findIt != m_entities.end())
	{
		m_entities.erase(findIt);
		delete entity;
	}

	return *this;
}

Scene & Scene::erase(PointLight * pointLight)
{
	auto findIt = std::find(m_pointLights.begin(), m_pointLights.end(), pointLight);

	if (findIt != m_pointLights.end())
	{
		delete *findIt;
		m_pointLights.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(DirectionalLight * directionalLight)
{
	auto findIt = std::find(m_directionalLights.begin(), m_directionalLights.end(), directionalLight);

	if (findIt != m_directionalLights.end())
	{
		delete *findIt;
		m_directionalLights.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(SpotLight * spotLight)
{
	auto findIt = std::find(m_spotLights.begin(), m_spotLights.end(), spotLight);

	if (findIt != m_spotLights.end())
	{
		delete *findIt;
		m_spotLights.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(Collider * collider)
{
	auto findIt = std::find(m_colliders.begin(), m_colliders.end(), collider);

	if (findIt != m_colliders.end())
	{
		delete *findIt;
		m_colliders.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(MeshRenderer * meshRenderer)
{
	//Add this components to renderables :
	IRenderableComponent* asRenderable = static_cast<IRenderableComponent*>(meshRenderer);
	if (asRenderable->getDrawableCount() > 0)
		m_renderables.remove(asRenderable, asRenderable->getDrawable(0).getVisualBoundingBox());

	auto findIt = std::find(m_meshRenderers.begin(), m_meshRenderers.end(), meshRenderer);

	if (findIt != m_meshRenderers.end())
	{
		delete meshRenderer;
		m_meshRenderers.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(Physic::Flag * flag)
{
	//Add this components to renderables :
	IRenderableComponent* asRenderable = static_cast<IRenderableComponent*>(flag);
	if (asRenderable->getDrawableCount() > 0)
		m_renderables.remove(asRenderable, asRenderable->getDrawable(0).getVisualBoundingBox());

	auto findIt = std::find(m_flags.begin(), m_flags.end(), flag);

	if (findIt != m_flags.end())
	{
		delete flag;
		m_flags.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(Physic::ParticleEmitter * particleEmitter)
{
	//Add this components to renderables :
	IRenderableComponent* asRenderable = static_cast<IRenderableComponent*>(particleEmitter);
	if (asRenderable->getDrawableCount() > 0)
		m_renderables.remove(asRenderable, asRenderable->getDrawable(0).getVisualBoundingBox());

	auto findIt = std::find(m_particleEmitters.begin(), m_particleEmitters.end(), particleEmitter);

	if (findIt != m_particleEmitters.end())
	{
		delete particleEmitter;
		m_particleEmitters.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(PathPoint * pathPoint)
{
	m_pathManager.erase(pathPoint);
	return *this;
}

Scene & Scene::erase(Billboard * billboard)
{
	//Add this components to renderables :
	IRenderableComponent* asRenderable = static_cast<IRenderableComponent*>(billboard);
	if (asRenderable->getDrawableCount() > 0)
		m_renderables.remove(asRenderable, asRenderable->getDrawable(0).getVisualBoundingBox());

	auto findIt = std::find(m_billboards.begin(), m_billboards.end(), billboard);

	if (findIt != m_billboards.end())
	{
		delete billboard;
		m_billboards.erase(findIt);
	}

	return *this;
}


Scene & Scene::erase(Camera * camera)
{
	auto findIt = std::find(m_cameras.begin(), m_cameras.end(), camera);

	if (findIt != m_cameras.end())
	{
		delete camera;
		m_cameras.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(Physic::WindZone * windZone)
{
	auto findIt = std::find(m_windZones.begin(), m_windZones.end(), windZone);

	if (findIt != m_windZones.end())
	{
		delete windZone;
		m_windZones.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(Rigidbody* rigidbody)
{
	auto findIt = std::find(m_rigidbodies.begin(), m_rigidbodies.end(), rigidbody);

	if (findIt != m_rigidbodies.end())
	{
		delete rigidbody;
		m_rigidbodies.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(Animator * animator)
{
	auto findIt = std::find(m_animators.begin(), m_animators.end(), animator);

	if (findIt != m_animators.end())
	{
		delete animator;
		m_animators.erase(findIt);
	}

	return *this;
}

Scene & Scene::erase(CharacterController * characterController)
{
	auto findIt = std::find(m_characterControllers.begin(), m_characterControllers.end(), characterController);

	if (findIt != m_characterControllers.end())
	{
		delete characterController;
		m_characterControllers.erase(findIt);
	}

	return *this;
}

Scene& Scene::erase(Behavior* behavior)
{
	auto findIt = std::find(m_behaviors.begin(), m_behaviors.end(), behavior);

	if (findIt != m_behaviors.end())
	{
		delete behavior;
		m_behaviors.erase(findIt);
	}

	return *this;
}

void Scene::computeCulling()
{
	for (auto& camera : m_cameras)
	{
		if(camera->getIsActive())
			camera->computeCulling(m_renderables);
	}
}

void Scene::computeCullingForSingleCamera(BaseCamera& camera)
{
	camera.computeCulling(m_renderables);
}

void Scene::render(BaseCamera& camera)
{
	//[DEPRECATED]
	//m_renderer->render(camera, m_meshRenderers, m_pointLights, m_directionalLights, m_spotLights, m_terrain, m_skybox, m_flags, m_billboards, m_particleEmitters, nullptr);
	m_renderer->render(camera, m_pointLights, m_directionalLights, m_spotLights);
}

void Scene::render(BaseCamera& camera, DebugDrawRenderer& debugDrawer)
{
	//[DEPRECATED]
	//m_renderer->render(camera, m_meshRenderers, m_pointLights, m_directionalLights, m_spotLights, m_terrain, m_skybox, m_flags, m_billboards, m_particleEmitters, &debugDrawer);
	m_renderer->render(camera, m_pointLights, m_directionalLights, m_spotLights, &debugDrawer); 
}

void Scene::renderColliders(const BaseCamera & camera)
{
	if(m_areCollidersVisible)
		m_renderer->debugDrawColliders(camera, m_entities);
}

void Scene::renderDebugLights(const BaseCamera & camera)
{
	if(m_areLightsBoundingBoxVisible)
		m_renderer->debugDrawLights(camera, m_pointLights, m_spotLights);
}

void Scene::renderPaths(const BaseCamera& camera)
{
	m_pathManager.render(camera);
}

void Scene::renderDebugOctrees(const BaseCamera & camera)
{
	if (m_areOctreesVisible)
	{
		OctreeDrawer::get().render(camera.getProjectionMatrix(), camera.getViewMatrix());
		OctreeDrawer::get().clear();
	}
}

void Scene::renderDebugPhysic(const BaseCamera & camera)
{
	if (m_isDebugPhysicVisible)
		m_physicManager->debugDraw(camera.getProjectionMatrix(), camera.getViewMatrix());
}

void Scene::updatePhysic(float deltaTime, const BaseCamera& camera)
{
	m_physicManager->update(deltaTime, camera, m_flags, m_terrain, m_windZones, m_particleEmitters);
}

void Scene::updatePhysic(float deltaTime, const BaseCamera& camera, bool updateInEditMode)
{
	m_physicManager->update(deltaTime, camera, m_flags, m_terrain, m_windZones, m_particleEmitters, updateInEditMode);
}

void Scene::updateAnimations(float time)
{
	// TODO animatorManager ? 
	for (int i = 0; i < m_animators.size(); i++) {
		m_animators[i]->updateAnimations(time);
	}
}

void Scene::updateControllers(float deltaTime)
{ 
	// TODO controllerManager ? 
	for (auto& controller : m_characterControllers) {
		controller->update(deltaTime);
	}
}

void Scene::updateBehaviours()
{
	m_behaviorManager.update(*this, m_behaviors);
	m_behaviorManager.updateCoroutines(m_entities);

	//TODO speed up this process :
	for (auto& entity : m_entities) {
		if (entity->getCollisionState() == Entity::CollisionState::END)
			entity->resetCollision();
	}
}

void Scene::toggleColliderVisibility()
{
	m_areCollidersVisible = !m_areCollidersVisible;
}

void Scene::toggleDebugDeferredVisibility()
{
	m_isDebugDeferredVisible = !m_isDebugDeferredVisible;
}

void Scene::toggleLightsBoundingBoxVisibility()
{
	m_areLightsBoundingBoxVisible = !m_areLightsBoundingBoxVisible;
}

void Scene::toggleOctreesVisibility()
{
	m_areOctreesVisible = !m_areOctreesVisible;
}

void Scene::toggleDebugPhysicVisibility()
{
	m_isDebugPhysicVisible = !m_isDebugPhysicVisible;
}

bool Scene::getAreCollidersVisible() const
{
	return m_areCollidersVisible;
}

bool Scene::getIsDebugDeferredVisible() const
{
	return m_isDebugDeferredVisible;
}

bool Scene::getAreLightsBoundingBoxVisible() const
{
	return m_areLightsBoundingBoxVisible;
}

bool Scene::getAreOctreesVisible() const
{
	return m_areOctreesVisible;
}

bool Scene::getIsDebugPhysicVisible() const
{
	return m_isDebugPhysicVisible;
}

void Scene::setAreCollidersVisible(bool value)
{
	m_areCollidersVisible = value;
}

void Scene::setIsDebugDeferredVisible(bool value)
{
	m_isDebugDeferredVisible = value;
}

void Scene::setAreLightsBoundingBoxVisible(bool value)
{
	m_areLightsBoundingBoxVisible = value;
}

void Scene::setAreOctreesVisible(bool value)
{
	m_areOctreesVisible = value;
}

void Scene::setIsDebugPhysicVisible(bool value)
{
	m_isDebugPhysicVisible = value;
}

Terrain& Scene::getTerrain()
{
	return m_terrain;
}

Skybox& Scene::getSkybox()
{
	return m_skybox;
}

PathManager & Scene::getPathManager()
{
	return m_pathManager;
}

Renderer& Scene::getRenderer()
{
	return *m_renderer;
}

Physic::PhysicManager& Scene::getPhysicManager()
{
	return *m_physicManager;
}

std::string Scene::getName() const
{
	return m_name;
}

void Scene::resolveEntityChildSaving(Json::Value & rootComponent, Entity* currentEntity) 
{
	rootComponent["isRootEntity"] = false;
	rootComponent["childCount"] = currentEntity->getChildCount();
	currentEntity->save(rootComponent);
	for (int i = 0; i < currentEntity->getChildCount(); i++) {
		resolveEntityChildSaving(rootComponent["childs"][i], currentEntity->getChild(i));
	}
}

void Scene::save(const FileHandler::CompletePath& path)
{
	Json::Value root;

	root["entityCount"] = m_entities.size();
	for (int i = 0; i < m_entities.size(); i++)
	{
		if (!m_entities[i]->hasParent()) {
			root["entities"][i]["isRootEntity"] = true;
			root["entities"][i]["childCount"] = m_entities[i]->getChildCount();
			m_entities[i]->save(root["entities"][i]);
			for (int j = 0; j < m_entities[i]->getChildCount(); j++) {
				resolveEntityChildSaving(root["entities"][i]["childs"][j], m_entities[i]->getChild(j));
			}
		}
	}

	//TODO
	m_terrain.save(root["terrain"]);
	m_skybox.save(root["skybox"]);
	
	//DEBUG
	//std::cout << root;

	std::ofstream stream;
	stream.open(path.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't save scene at path : " << path.toString() << std::endl;
		return;
	}
	//save scene
	stream << root;
}

void Scene::resolveEntityChildPreLoading(Json::Value & rootComponent, Entity* currentEntity)
{
	int childcount = rootComponent.get("childCount", 0).asInt();
	for (int j = 0; j < childcount; j++) {
		Entity* newEntity = new Entity(this);
		currentEntity->addChild(newEntity);
		resolveEntityChildPreLoading(rootComponent["childs"][j], newEntity);
	}
}

void Scene::resolveEntityChildLoading(Json::Value & rootComponent, Entity* currentEntity)
{
	int childcount = rootComponent.get("childCount", 0).asInt();
	for (int j = 0; j < childcount; j++) {
		currentEntity->getChild(j)->load(rootComponent["childs"][j]);
		resolveEntityChildLoading(rootComponent["childs"][j], currentEntity->getChild(j));
	}
}

void Scene::load(const FileHandler::CompletePath& path)
{
	Json::Value root;

	std::ifstream stream;
	stream.open(path.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't load scene at path : " << path.toString() << std::endl;
		return;
	}
	stream >> root;
	
	int entityCount = root.get("entityCount", 0).asInt();
	assert(m_entities.size() == 0);
	for (int i = 0; i < entityCount; i++) {
		if (root["entities"][i].get("isRootEntity", false).asBool() == true) {
			auto newEntity = new Entity(this);
			resolveEntityChildPreLoading(root["entities"][i], newEntity);
		}
	}
	for (int i = 0; i < entityCount; i++) {
		if (root["entities"][i].get("isRootEntity", false).asBool() == true) {
			m_entities[i]->load(root["entities"][i]);
			resolveEntityChildLoading(root["entities"][i], m_entities[i]);
		}
	}

	//TODO
	m_terrain.load(root["terrain"]);
	//m_terrain.initPhysics(m_physicManager.getBulletDynamicSimulation()); //TODO automatize this process in loading ? 
	m_skybox.load(root["skybox"]);

}

BaseCamera* Scene::getMainCamera() const
{
	return m_cameras.size() > 0 ? m_cameras[0] : nullptr;
}

void Scene::onViewportResized(const glm::vec2 & newSize)
{
	for (auto& camera : m_cameras)
	{
		camera->onViewportResized(newSize);
	}
}

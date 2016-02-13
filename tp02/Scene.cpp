#include "Scene.h"


Scene::Scene(Renderer * renderer) : m_renderer(renderer), m_areCollidersVisible(true), m_isDebugDeferredVisible(true)
{
}

Scene::~Scene()
{
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
	return *this;
}

Scene& Scene::add(Physic::Flag * flag)
{
	m_flags.push_back(flag);
	return *this;
}

Scene & Scene::add(Physic::ParticleEmitter * particleEmitter)
{
	m_particleEmitters.push_back(particleEmitter);
	return *this;
}

Scene & Scene::add(PathPoint * pathPoint)
{
	m_pathManager.add(pathPoint);
	return *this;
}

Scene& Scene::erase(Entity * entity)
{
	auto findIt = std::find(m_entities.begin(), m_entities.end(), entity);

	if (findIt != m_entities.end())
	{
		delete *findIt;
		m_entities.erase(findIt);
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
	m_pathManager.remove(pathPoint);
	return *this;
}

void Scene::render(const Camera& camera)
{
	m_renderer->render(camera, m_meshRenderers, m_pointLights, m_directionalLights, m_spotLights, m_terrain, m_skybox, m_flags);
}

void Scene::renderColliders(const Camera & camera)
{
	if(m_areCollidersVisible)
		m_renderer->debugDrawColliders(camera, m_entities);
}

void Scene::renderDebugDeferred()
{
	if(m_isDebugDeferredVisible)
		m_renderer->debugDrawDeferred();
}

void Scene::renderDebugLights(const Camera & camera)
{
	if(m_areLightsBoundingBoxVisible)
		m_renderer->debugDrawLights(camera, m_pointLights, m_spotLights);
}

void Scene::updatePhysic(float deltaTime)
{
	m_physicManager.update(deltaTime, m_flags);
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

void Scene::culling(const Camera & camera)
{
	m_renderer->updateCulling(camera, m_pointLights, m_spotLights);
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

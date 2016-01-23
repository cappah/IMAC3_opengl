#include "Scene.h"


Scene::Scene(Renderer * renderer) : m_renderer(renderer)
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

Scene & Scene::erase(Entity * entity)
{
	auto findIt = std::find(m_entities.begin(), m_entities.end(), entity);

	if (findIt != m_entities.end())
	{
		m_entities.erase(findIt);
		delete *findIt;
	}

	return *this;
}

Scene & Scene::erase(PointLight * pointLight)
{
	auto findIt = std::find(m_pointLights.begin(), m_pointLights.end(), pointLight);

	if (findIt != m_pointLights.end())
	{
		m_pointLights.erase(findIt);
		delete *findIt;
	}

	return *this;
}

Scene & Scene::erase(DirectionalLight * directionalLight)
{
	auto findIt = std::find(m_directionalLights.begin(), m_directionalLights.end(), directionalLight);

	if (findIt != m_directionalLights.end())
	{
		m_directionalLights.erase(findIt);
		delete *findIt;
	}

	return *this;
}

Scene & Scene::erase(SpotLight * spotLight)
{
	auto findIt = std::find(m_spotLights.begin(), m_spotLights.end(), spotLight);

	if (findIt != m_spotLights.end())
	{
		m_spotLights.erase(findIt);
		delete *findIt;
	}

	return *this;
}

Scene & Scene::erase(Collider * collider)
{
	auto findIt = std::find(m_colliders.begin(), m_colliders.end(), collider);

	if (findIt != m_colliders.end())
	{
		m_colliders.erase(findIt);
		delete *findIt;
	}

	return *this;
}

Scene & Scene::erase(MeshRenderer * meshRenderer)
{
	auto findIt = std::find(m_meshRenderers.begin(), m_meshRenderers.end(), meshRenderer);

	if (findIt != m_meshRenderers.end())
	{
		m_meshRenderers.erase(findIt);
		delete *findIt;
	}

	return *this;
}

void Scene::render(const Camera& camera)
{
	m_renderer->render(camera, m_meshRenderers, m_pointLights, m_directionalLights, m_spotLights);
}

void Scene::renderColliders(const Camera & camera)
{
	m_renderer->debugDrawColliders(camera, m_entities);
}

void Scene::renderDebugDeferred()
{
	m_renderer->debugDrawDeferred();
}

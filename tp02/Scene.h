#pragma once

#include <vector>

#include "Entity.h"
#include "Lights.h"
#include "Collider.h"

#include "LightManager.h"
#include "Renderer.h"
#include "Camera.h"

class Scene
{
private:
	//entities : 
	std::vector<Entity*> m_entities;

	//lights : 
	std::vector<PointLight*> m_pointLights;
	std::vector<DirectionalLight*> m_directionalLights;
	std::vector<SpotLight*> m_spotLights;

	//colliders : 
	std::vector<Collider*> m_colliders;

	//meshRenderers : 
	std::vector<MeshRenderer*> m_meshRenderers;

	Renderer* m_renderer;

public:
	Scene(Renderer * renderer);
	~Scene();

	std::vector<Entity*>& getEntities();

	Scene& add(Entity* entity);
	Scene& add(PointLight* pointLight);
	Scene& add(DirectionalLight* directionalLight);
	Scene& add(SpotLight* spotLight);
	Scene& add(Collider* collider);
	Scene& add(MeshRenderer* meshRenderer);

	Scene& erase(Entity* entity);
	Scene& erase(PointLight* pointLight);
	Scene& erase(DirectionalLight* directionalLight);
	Scene& erase(SpotLight* spotLight);
	Scene& erase(Collider* collider);
	Scene& erase(MeshRenderer* meshRenderer);

	void render(const Camera& camera);
	void renderColliders(const Camera& camera);
	void renderDebugDeferred();
};

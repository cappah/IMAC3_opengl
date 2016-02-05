#pragma once

#include <vector>

#include "Entity.h"
#include "Lights.h"
#include "Collider.h"
#include "Flag.h"
#include "PhysicManager.h"

#include "LightManager.h"
#include "Renderer.h"
#include "Camera.h"
#include "Terrain.h"
#include "Skybox.h"
#include "PhysicManager.h"

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

	//flag : 
	std::vector<Physic::Flag*> m_flags;

	Renderer* m_renderer;
	Physic::PhysicManager m_physicManager;

	bool m_areCollidersVisible;
	bool m_isDebugDeferredVisible;
	bool m_areLightsBoundingBoxVisible;

	Terrain m_terrain;
	Skybox m_skybox;

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
	Scene& add(Physic::Flag* flag);

	Scene& erase(Entity* entity);
	Scene& erase(PointLight* pointLight);
	Scene& erase(DirectionalLight* directionalLight);
	Scene& erase(SpotLight* spotLight);
	Scene& erase(Collider* collider);
	Scene& erase(MeshRenderer* meshRenderer);
	Scene& erase(Physic::Flag* flag);

	void render(const Camera& camera);
	void renderColliders(const Camera& camera);
	void renderDebugDeferred();
	void renderDebugLights(const Camera& camera);

	void updatePhysic(float deltaTime);

	void toggleColliderVisibility();
	void toggleDebugDeferredVisibility();
	void toggleLightsBoundingBoxVisibility();
	bool getAreCollidersVisible() const;
	bool getIsDebugDeferredVisible() const;
	bool getAreLightsBoundingBoxVisible() const;
	void setAreCollidersVisible(bool value);
	void setIsDebugDeferredVisible(bool value);
	void setAreLightsBoundingBoxVisible(bool value);

	void culling(const Camera& camera);

	Terrain& getTerrain();
	Skybox& getSkybox();
};


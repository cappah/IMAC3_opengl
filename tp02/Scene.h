#pragma once

#include <vector>

#include "Entity.h"
#include "Lights.h"
#include "Collider.h"
#include "Flag.h"
#include "ParticleEmitter.h"
#include "PhysicManager.h"

#include "LightManager.h"
#include "Renderer.h"
#include "Camera.h"
#include "Terrain.h"
#include "Skybox.h"

class Scene
{
private:
	//entities : 
	std::vector<Entity*> m_entities;

	//components : 
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

	//particles : 
	std::vector<Physic::ParticleEmitter*> m_particleEmitters;

	//special componants : 
	//terrain : 
	Terrain m_terrain;

	//skybox : 
	Skybox m_skybox;


	//systems : 
	Renderer* m_renderer;
	Physic::PhysicManager m_physicManager;
	//TODO, after particles
	//CloudSystem m_cloudSystem;
	//WeatherSystem m_weatherSystem;

	//parameters : 
	bool m_areCollidersVisible;
	bool m_isDebugDeferredVisible;
	bool m_areLightsBoundingBoxVisible;


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
	Scene& add(Physic::ParticleEmitter* particleEmitter);

	Scene& erase(Entity* entity);
	Scene& erase(PointLight* pointLight);
	Scene& erase(DirectionalLight* directionalLight);
	Scene& erase(SpotLight* spotLight);
	Scene& erase(Collider* collider);
	Scene& erase(MeshRenderer* meshRenderer);
	Scene& erase(Physic::Flag* flag);
	Scene& erase(Physic::ParticleEmitter* particleEmitter);

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


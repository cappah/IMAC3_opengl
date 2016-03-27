#pragma once

#include <vector>

#include "Entity.h"
#include "Lights.h"
#include "Collider.h"
#include "Flag.h"
#include "ParticleEmitter.h"
#include "PhysicManager.h"
#include "PathManager.h"
#include "Billboard.h"

#include "LightManager.h"
#include "Renderer.h"
#include "Camera.h"
#include "Terrain.h"
#include "Skybox.h"

#include "jsoncpp/json/json.h"
#include <iostream>
#include <fstream>

class Scene
{
private:
	//scene name :
	std::string m_name;

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

	//billboards : 
	std::vector<Billboard*> m_billboards;

	//cameras : 
	std::vector<Camera*> m_cameras;
	
	//windZones : 
	std::vector<Physic::WindZone*> m_windZones;

	//special componants : 
	//terrain : 
	Terrain m_terrain;

	//skybox : 
	Skybox m_skybox;


	//systems : 
	Renderer* m_renderer;
	Physic::PhysicManager m_physicManager;
	PathManager m_pathManager;
	//TODO
	//CloudSystem m_cloudSystem;

	//parameters : 
	bool m_areCollidersVisible;
	bool m_isDebugDeferredVisible;
	bool m_areLightsBoundingBoxVisible;
	bool m_areOctreesVisible;


public:
	Scene(Renderer* renderer, const std::string& sceneName = "defaultScene");
	~Scene();

	void clear();

	std::vector<Entity*>& getEntities();

	Scene& add(Entity* entity);
	Scene& add(PointLight* pointLight);
	Scene& add(DirectionalLight* directionalLight);
	Scene& add(SpotLight* spotLight);
	Scene& add(Collider* collider);
	Scene& add(MeshRenderer* meshRenderer);
	Scene& add(Physic::Flag* flag);
	Scene& add(Physic::ParticleEmitter* particleEmitter);
	Scene& add(PathPoint* pathPoint);
	Scene& add(Billboard* billboard);
	Scene& add(Camera* camera);
	Scene& add(Physic::WindZone* windZone);

	Scene& erase(Entity* entity);
	Scene& erase(PointLight* pointLight);
	Scene& erase(DirectionalLight* directionalLight);
	Scene& erase(SpotLight* spotLight);
	Scene& erase(Collider* collider);
	Scene& erase(MeshRenderer* meshRenderer);
	Scene& erase(Physic::Flag* flag);
	Scene& erase(Physic::ParticleEmitter* particleEmitter);
	Scene& erase(PathPoint* pathPoint);
	Scene& erase(Billboard* billboard);
	Scene& erase(Camera* camera);
	Scene& erase(Physic::WindZone* windZone);

	void render(const BaseCamera& camera);
	void renderColliders(const BaseCamera& camera);
	void renderDebugDeferred();
	void renderDebugLights(const BaseCamera& camera);
	void renderPaths(const BaseCamera& camera);
	void renderDebugOctrees(const BaseCamera& camera);

	void updatePhysic(float deltaTime, const BaseCamera& camera);

	void toggleColliderVisibility();
	void toggleDebugDeferredVisibility();
	void toggleLightsBoundingBoxVisibility();
	void toggleOctreesVisibility();
	bool getAreCollidersVisible() const;
	bool getIsDebugDeferredVisible() const;
	bool getAreLightsBoundingBoxVisible() const;
	bool getAreOctreesVisible() const;
	void setAreCollidersVisible(bool value);
	void setIsDebugDeferredVisible(bool value);
	void setAreLightsBoundingBoxVisible(bool value);
	void setAreOctreesVisible(bool value);

	void culling(const BaseCamera& camera);

	Terrain& getTerrain();
	Skybox& getSkybox();
	PathManager& getPathManager();
	Renderer& getRenderer();

	std::string getName() const;

	void save(const std::string& path);
	void load(const std::string& path);

	BaseCamera* getMainCamera() const;

};


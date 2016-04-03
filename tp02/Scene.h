#pragma once

#include <vector>

#include "Entity.h"
#include "Lights.h"
#include "Collider.h"
#include "Flag.h"
#include "ParticleEmitter.h"
#include "Billboard.h"
#include "Rigidbody.h"
#include "Camera.h"

#include "PhysicManager.h"
#include "PathManager.h"
#include "LightManager.h"
#include "Renderer.h"
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

	//rigidbodies : 
	std::vector<Rigidbody*> m_rigidbodies;

	//special components : 
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
	bool m_isDebugPhysicVisible;


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
	Scene& add(Rigidbody* rigidbody);

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
	Scene& erase(Rigidbody* rigidbody);

	void render(const BaseCamera& camera);
	void renderColliders(const BaseCamera& camera);
	void renderDebugDeferred();
	void renderDebugLights(const BaseCamera& camera);
	void renderPaths(const BaseCamera& camera);
	void renderDebugOctrees(const BaseCamera& camera);
	void renderDebugPhysic(const BaseCamera& camera);

	void updatePhysic(float deltaTime, const BaseCamera& camera);
	void updatePhysic(float deltaTime, const BaseCamera& camera, bool updateInEditMode);

	void toggleColliderVisibility();
	void toggleDebugDeferredVisibility();
	void toggleLightsBoundingBoxVisibility();
	void toggleOctreesVisibility();
	void toggleDebugPhysicVisibility();
	bool getAreCollidersVisible() const;
	bool getIsDebugDeferredVisible() const;
	bool getAreLightsBoundingBoxVisible() const;
	bool getAreOctreesVisible() const;
	bool getIsDebugPhysicVisible() const;
	void setAreCollidersVisible(bool value);
	void setIsDebugDeferredVisible(bool value);
	void setAreLightsBoundingBoxVisible(bool value);
	void setAreOctreesVisible(bool value);
	void setIsDebugPhysicVisible(bool value);

	void culling(const BaseCamera& camera);

	Terrain& getTerrain();
	Skybox& getSkybox();
	PathManager& getPathManager();
	Renderer& getRenderer();
	Physic::PhysicManager& getPhysicManager();

	std::string getName() const;

	//helper to load entities, for making links between entities and their childs
	void resolveEntityChildSaving(Json::Value & rootComponent, Entity* currentEntity);
	void resolveEntityChildPreLoading(Json::Value & rootComponent, Entity* currentEntity);
	void resolveEntityChildLoading(Json::Value & rootComponent, Entity* currentEntity);
	void save(const std::string& path);
	void load(const std::string& path);

	BaseCamera* getMainCamera() const;

};


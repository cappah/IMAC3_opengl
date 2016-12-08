#pragma once

#include <vector>
#include <unordered_map>
#include "Component.h"
#include "Lights.h"
#include "Collider.h"
#include "Flag.h"
#include "ParticleEmitter.h"
#include "Billboard.h"
#include "Rigidbody.h"
#include "Camera.h"
#include "Behavior.h"
#include "Animator.h"
#include "CharacterController.h"

class SceneAccessor
{
private:
	Scene* m_scene;

public:

	SceneAccessor(Scene* scene) : m_scene(scene)
	{}

	SceneAccessor& addToScene(Entity* entity);
	SceneAccessor& eraseFromScene(Entity* entity);

	template<typename T>
	SceneAccessor& addToScene(Component* component)
	{
		std::vector<T*>& vectorOfComponents = *static_cast<std::vector<T*>*>(getSceneComponentContainer(0/*component->getClassa()*/));

		auto findIt = std::find(vectorOfComponents.begin(), vectorOfComponents.end(), component);

		if (findIt == vectorOfComponents.end())
			vectorOfComponents.push_back(static_cast<T*>(component));
		else
			PRINT_WARNING("You are trying to put a component multiple time into the scene.");

		return *this;
	}

	template<typename T>
	SceneAccessor& eraseFromScene(Component* component)
	{
		std::vector<T*>& vectorOfComponents = *static_cast<std::vector<T*>*>(getSceneComponentContainer(0/*component->getClassa()*/));

		delete component;
		vectorOfComponents.erase(std::remove(vectorOfComponents.begin(), vectorOfComponents.end(), component));

		return *this;
	}

private:
	void* getSceneComponentContainer(int componentClassId);
};
#include "Behavior.h"
//forwards : 
#include "BehaviorFactory.h"
#include "Scene.h"

Behavior::Behavior(/*const std::type_index& type*/) : Component(ComponentType::BEHAVIOR), m_isInitialized(false)
{
	//static bool initialized = false;
	//if (!initialized) {
	//	BehaviorFactory::get().add(type, this);
	//	initialized = true;
	//}
}


Behavior::~Behavior()
{
}


void Behavior::start(Scene& scene)
{
	//to override
}

void Behavior::update(Scene& scene)
{
	//to override
}

void Behavior::onColliderEnter(Collider & other)
{
	//to override
}

void Behavior::onColliderExit(Collider & other)
{
	//to override
}

void Behavior::onColliderStay(Collider & other)
{
	//to override
}

void Behavior::drawUI(Scene & scene)
{
	//to override
}

void Behavior::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

void Behavior::addToScene(Scene & scene)
{
	scene.add(this);
}

void Behavior::eraseFromEntity(Entity & entity)
{
	entity.erase(this);
}

void Behavior::addToEntity(Entity & entity)
{
	entity.add(this);
}

void Behavior::save(Json::Value & entityRoot) const
{
	Component::save(entityRoot);
	entityRoot["typeIndexName"] = getTypeIndex().name();
}

void Behavior::load(Json::Value & entityRoot)
{
	Component::load(entityRoot);
}

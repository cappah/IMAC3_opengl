#include "CharacterController.h"
//forwards :
#include "Scene.h"
#include "Entity.h"

CharacterController::CharacterController(): Component(ComponentType::CHARACTER_CONTROLLER)
{
}


CharacterController::~CharacterController()
{
}

void CharacterController::drawUI(Scene & scene)
{
	//TODO
}

void CharacterController::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

Component* CharacterController::clone(Entity* entity)
{
	CharacterController* newCharacterController = new CharacterController(*this);

	newCharacterController->attachToEntity(entity);

	return newCharacterController;
}

void CharacterController::addToScene(Scene& scene)
{
	scene.add(this);
}

void CharacterController::addToEntity(Entity & entity)
{
	entity.add(this);
}

void CharacterController::eraseFromEntity(Entity& entity)
{
	entity.erase(this);
}
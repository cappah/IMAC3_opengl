#include "ParticleEmitter.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

namespace Physic {


	ParticleEmitter::ParticleEmitter() : Component(PARTICLE_EMITTER)
	{
		//TODO
	}


	ParticleEmitter::~ParticleEmitter()
	{
		//TODO
	}

	void ParticleEmitter::drawUI(Scene& scene)
	{
		if (ImGui::CollapsingHeader("particle emitter"))
		{
			//TODO
		}
	}

	void ParticleEmitter::eraseFromScene(Scene & scene)
	{
		scene.erase(this);
	}

	void ParticleEmitter::addToScene(Scene & scene)
	{
		scene.add(this);
	}

	Component * ParticleEmitter::clone(Entity * entity)
	{
		ParticleEmitter* newParticleEmitter = new ParticleEmitter(*this);

		newParticleEmitter->attachToEntity(entity);

		return newParticleEmitter;
	}

}

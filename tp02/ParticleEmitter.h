#pragma once

#include "Mesh.h"
#include "Materials.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Component.h"

namespace Physic{

	class ParticleEmitter : public Component
	{
	private:
		//TODO
	public:
		ParticleEmitter();
		~ParticleEmitter();

		//TODO

		//draw the ui of particle emitter
		virtual void drawUI(Scene& scene) override;
		//herited from Component
		virtual void eraseFromScene(Scene & scene) override;
		virtual void addToScene(Scene & scene) override;
		virtual Component * clone(Entity * entity) override;
		virtual void addToEntity(Entity& entity) override;
		virtual void eraseFromEntity(Entity& entity) override;

		virtual void save(Json::Value& rootComponent) const override;
		virtual void load(Json::Value& rootComponent) override;
	};

}


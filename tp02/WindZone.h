#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "CSpline.h"
#include "Component.h"

namespace Physic {

	class WindZone : public Component
	{
	private:
		Math::CSpline<float> m_cspline;
		glm::vec3 m_direction;
		float m_amplitude;
		float m_randomFactor;
		float m_frequency;

	public:
		WindZone();
		~WindZone();

		glm::vec3 getForce(float t);

		void updateSpline();

		float getAmplitude() const;
		glm::vec3 getDirection() const;
		float getRandomFactor() const;
		float getFrequency() const;

		void setAmplitude(float amplitude);
		void setDirection(glm::vec3 direction);
		void setRandomFactor(float randomFactor);
		void setFrequency(float frequency);

		virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1, 1, 1), const glm::quat& rotation = glm::quat()) override;
		virtual void drawUI(Scene & scene) override;
		virtual void eraseFromScene(Scene & scene) override;
		virtual void addToScene(Scene & scene) override;
		virtual Component * clone(Entity * entity) override;
	};
}


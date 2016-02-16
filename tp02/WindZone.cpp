#include "WindZone.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

namespace Physic {

	WindZone::WindZone() : Component(ComponentType::WIND_ZONE), m_direction(0, 0, 1), m_amplitude(1), m_frequency(1), m_randomFactor(0)
	{
		for (int i = 0; i < 10; i++)
		{
			float direction = (i % 2 == 0) ? 1 : -1;
			float newPoint = direction*m_amplitude*0.5f - m_randomFactor * direction * (float)(rand() % 100) / 100.f;
			m_cspline.push_back(newPoint);
		}
	}


	WindZone::~WindZone()
	{
	}

	glm::vec3 WindZone::getForce(float t)
	{
		t *= m_frequency;
		t *= 1000.f;
		t = ((int)t) % 1000;
		t *= 0.001f;
		return m_direction * m_cspline.get(t);
	}

	void WindZone::updateSpline()
	{
		for (int i = 0; i < 10; i++)
		{
			float direction = (i % 2 == 0) ? 1 : -1;
			float newPoint = m_amplitude - m_randomFactor * direction * (float)(rand() % 100) / 100.f;
			m_cspline[i] = (newPoint);
		}
	}

	float WindZone::getAmplitude() const
	{
		return m_amplitude;
	}

	glm::vec3 WindZone::getDirection() const
	{
		return m_direction;
	}

	float WindZone::getRandomFactor() const
	{
		return m_randomFactor;
	}

	float WindZone::getFrequency() const
	{
		return m_frequency;
	}

	void WindZone::setAmplitude(float amplitude)
	{
		m_amplitude = amplitude;
	}

	void WindZone::setDirection(glm::vec3 direction)
	{
		m_direction = direction;
	}

	void WindZone::setRandomFactor(float randomFactor)
	{
		m_randomFactor = randomFactor;
	}

	void WindZone::setFrequency(float frequency)
	{
		m_frequency = frequency;
	}

	void WindZone::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
	{
		m_direction = rotation * glm::vec3(0, 0, 1);
	}

	void WindZone::drawUI(Scene & scene)
	{
		if (ImGui::CollapsingHeader("wind zone"))
		{
			if (ImGui::InputFloat("amplitude", &m_amplitude))
				updateSpline();
			if (ImGui::InputFloat("frequency", &m_frequency))
				updateSpline();
			if (ImGui::InputFloat("random factor", &m_randomFactor))
				updateSpline();
		}
	}

	void WindZone::eraseFromScene(Scene & scene)
	{
		scene.erase(this);
	}

	void WindZone::addToScene(Scene & scene)
	{
		scene.add(this);
	}

	Component * WindZone::clone(Entity * entity)
	{
		WindZone* windZone = new WindZone(*this);

		windZone->attachToEntity(entity);

		return windZone;
	}
}

#include "WindZone.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

namespace Physic {

	WindZone::WindZone() : Component(ComponentType::WIND_ZONE), m_direction(0, 0, 1), m_amplitude(1), m_frequency(1), m_randomFactor(0), m_emissionType(EmissionType::DIRECTIONNAL), m_isAttenuated(false), m_radius(1.f)
	{
		//for UI : 
		m_emissionTypeNames = new const char*[2];
		m_emissionTypeNames[0] = "directionnal";
		m_emissionTypeNames[1] = "radial";


		for (int i = 0; i < 10; i++)
		{
			float direction = (i % 2 == 0) ? 1 : -1;
			float newPoint = direction*m_amplitude*0.5f - m_randomFactor * direction * (float)(rand() % 100) / 100.f;
			m_cspline.push_back(newPoint);
		}
	}


	WindZone::~WindZone()
	{
		delete[] m_emissionTypeNames;
	}

	//Warning : bypass the attenuation, force emissionType to be directional : 
	glm::vec3 WindZone::getForce(float t)
	{
		t *= m_frequency;
		t *= 1000.f;
		t = ((int)t) % 1000;
		t *= 0.001f;
		return m_direction * m_cspline.get(t);
	}

	glm::vec3 WindZone::getForce(float t, glm::vec3 position)
	{
		t *= m_frequency;
		t *= 1000.f;
		t = ((int)t) % 1000;
		t *= 0.001f;
		float distance = glm::length(position - m_position) + 0.000001f;
		
		float attenuation = m_isAttenuated ? std::max(0.f, (1 - distance / (m_radius + 0.0000001f))) : 1;

		glm::vec3 direction = (m_emissionType == EmissionType::DIRECTIONNAL) ? m_direction : glm::normalize(position - m_position);

		return direction * m_cspline.get(t) * attenuation;
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

	glm::vec3 WindZone::getPosition() const
	{
		return m_position;
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
		m_position = translation;
		m_direction = rotation * glm::vec3(0, 0, 1);
	}

	void WindZone::drawUI(Scene & scene)
	{
		if (ImGui::InputFloat("amplitude", &m_amplitude))
			updateSpline();
		if (ImGui::InputFloat("frequency", &m_frequency))
			updateSpline();
		if (ImGui::InputFloat("random factor", &m_randomFactor))
			updateSpline();
		int currentItemTypeEmission = (int)m_emissionType;
		if (ImGui::ListBox("emission type", &currentItemTypeEmission, m_emissionTypeNames, 2))
			m_emissionType = (EmissionType)currentItemTypeEmission;
		if (ImGui::RadioButton("attenuation", m_isAttenuated))
			m_isAttenuated = !m_isAttenuated;
		ImGui::SliderFloat("radius", &m_radius, 0.1f, 500.f);
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
	void WindZone::eraseFromEntity(Entity & entity)
	{
		entity.erase(this);
	}
	void WindZone::addToEntity(Entity & entity)
	{
		entity.add(this);
	}
}

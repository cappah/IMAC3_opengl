#include "ParticleEmitter.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

namespace Physic {


	ParticleEmitter::ParticleEmitter() : Component(PARTICLE_EMITTER), 
	m_maxParticleCount(10), m_aliveParticlesCount(0), m_lifeTimeInterval(3,5), m_initialVelocityInterval(0.1f, 0.5f), m_spawnFragment(0), m_particleCountBySecond(10), 
	m_translation(glm::vec3(0,0,0)),
	m_materialParticules(MaterialFactory::get().get<MaterialParticlesCPU>("particlesCPU")),
	//m_materialParticuleSimulation(MaterialFactory::get().get<MaterialParticleSimulation>("particleSimulation")),
	m_triangleIndex({ 0, 1, 2, 2, 3, 0 }),
	m_uvs({ 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f }), m_vertices({ -0.5, 0.0, -0.5, 0.5, 0.0, -0.5, 0.5, 0.0, 0.5, -0.5, 0.0, 0.5 }), 
	m_normals({ 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 }), m_particleTexture(TextureFactory::get().get("default"))
	{
		//add default color step :
		m_colorSteps_times.push_back(0);
		m_colorSteps_values.push_back(glm::vec4(1,0,0,1));

		//add default force step : 
		m_forceSteps_times.push_back(0);
		m_forceSteps_values.push_back(glm::vec3(0, 1, 0));

		//initialize system : 
		for (int i = 0; i < m_maxParticleCount; i++)
		{
			m_positions.push_back(glm::vec3(0, 0, 0));
			m_velocities.push_back(glm::vec3(0,0,0));
			m_forces.push_back(glm::vec3(0,0,0));
			m_elapsedTimes.push_back(0.f);
			m_lifeTimes.push_back(5.f);
			m_colors.push_back(glm::vec4(1,0,0,1));
			m_sizes.push_back(glm::vec2(1.f, 1.f));
		}

		//initialize vbos :
		initGl();
	}


	ParticleEmitter::~ParticleEmitter()
	{
		//TODO
	}

	void ParticleEmitter::initGl()
	{
		m_triangleCount = m_triangleIndex.size() / 3;

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_index);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangleIndex.size()*sizeof(int), &m_triangleIndex[0], GL_STATIC_DRAW);

		glGenBuffers(1, &m_vboVertices);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
		glEnableVertexAttribArray(VERTICES);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(float), &m_vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(VERTICES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

		glGenBuffers(1, &m_vboNormals);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
		glEnableVertexAttribArray(NORMALS);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(float), &m_normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);


		glGenBuffers(1, &m_vboUvs);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboUvs);
		glEnableVertexAttribArray(UVS);
		glBufferData(GL_ARRAY_BUFFER, m_uvs.size()*sizeof(float), &m_uvs[0], GL_STATIC_DRAW);
		glVertexAttribPointer(UVS, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);

		//instanced stuff :
		glGenBuffers(1, &m_vboPositions);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
		glEnableVertexAttribArray(POSITIONS);
		glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), &m_positions[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

		glGenBuffers(1, &m_vboColors);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
		glEnableVertexAttribArray(COLORS);
		glBufferData(GL_ARRAY_BUFFER, m_colors.size()*sizeof(glm::vec4), &m_colors[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);

		glGenBuffers(1, &m_vboSizes);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboSizes);
		glEnableVertexAttribArray(SIZES);
		glBufferData(GL_ARRAY_BUFFER, m_sizes.size()*sizeof(glm::vec2), &m_sizes[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(SIZES, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
	}

	void ParticleEmitter::swapParticles(int a_idx, int b_idx)
	{
		std::iter_swap(m_positions.begin() + a_idx, m_positions.begin() + b_idx);
		std::iter_swap(m_velocities.begin() + a_idx, m_velocities.begin() + b_idx);
		std::iter_swap(m_forces.begin() + a_idx, m_forces.begin() + b_idx);
		std::iter_swap(m_elapsedTimes.begin() + a_idx, m_elapsedTimes.begin() + b_idx);
		std::iter_swap(m_lifeTimes.begin() + a_idx, m_lifeTimes.begin() + b_idx);
		std::iter_swap(m_colors.begin() + a_idx, m_colors.begin() + b_idx);
		std::iter_swap(m_sizes.begin() + a_idx, m_sizes.begin() + b_idx);
	}

	glm::vec3 ParticleEmitter::getInternalParticleForce(float elapsedTime, float lifeTime, const glm::vec3 & position)
	{
		assert(m_forceSteps_times.size() == m_forceSteps_values.size());

		if (m_forceSteps_values.size() < 2 || lifeTime == 0) {
			if (m_forceSteps_values.size() > 0)
				return m_forceSteps_values[0];
			else
				return glm::vec3(0, 0, 0);
		}

		float timeRatio = elapsedTime / lifeTime;

		int stepIdx = 0;
		for (int i = 0; i < m_forceSteps_times.size() - 1; i++) {
			if (m_forceSteps_times[i] <= timeRatio && m_forceSteps_times[i + 1] > timeRatio) {
				stepIdx = i;
				elapsedTime -= m_forceSteps_times[i];
				elapsedTime *= m_forceSteps_times[i+1] - m_forceSteps_times[i];
				break;
			}
		}

		return  m_forceSteps_values[stepIdx] * (1 - elapsedTime) + m_forceSteps_values[stepIdx + 1] * elapsedTime;
	}

	glm::vec2 ParticleEmitter::getInternalParticleSize(float elapsedTime, float lifeTime, const glm::vec3 & position)
	{
		assert(m_sizeSteps_times.size() == m_sizeSteps_values.size());

		if (m_sizeSteps_values.size() < 2 || lifeTime == 0) {
			if (m_sizeSteps_values.size() > 0)
				return m_sizeSteps_values[0];
			else
				return glm::vec2(1, 1);
		}

		float timeRatio = elapsedTime / lifeTime;

		int stepIdx = 0;
		for (int i = 0; i < m_sizeSteps_times.size() - 1; i++) {
			if (m_sizeSteps_times[i] <= timeRatio && m_sizeSteps_times[i + 1] > timeRatio) {
				stepIdx = i;
				elapsedTime -= m_sizeSteps_times[i];
				elapsedTime *= m_sizeSteps_times[i + 1] - m_sizeSteps_times[i];
				break;
			}
		}

		return  m_sizeSteps_values[stepIdx] * (1 - elapsedTime) + m_sizeSteps_values[stepIdx + 1] * elapsedTime;
	}

	glm::vec4 ParticleEmitter::getInternalParticleColor(float elapsedTime, float lifeTime, const glm::vec3 & position)
	{
		assert(m_colorSteps_times.size() == m_colorSteps_values.size());

		if (m_colorSteps_values.size() < 2 || lifeTime == 0) {
			if (m_colorSteps_values.size() > 0)
				return m_colorSteps_values[0];
			else
				return glm::vec4(1, 1, 1, 1);
		}

		float timeRatio = elapsedTime / lifeTime;

		int stepIdx = 0;
		for (int i = 0; i < m_colorSteps_times.size() - 1; i++) {
			if (m_colorSteps_times[i] <= timeRatio && m_colorSteps_times[i + 1] > timeRatio) {
				stepIdx = i;
				elapsedTime -= m_colorSteps_times[i];
				elapsedTime *= m_colorSteps_times[i + 1] - m_colorSteps_times[i];
				break;
			}
		}

		return  m_colorSteps_values[stepIdx] * (1 - elapsedTime) + m_colorSteps_values[stepIdx + 1] * elapsedTime;
	}

	glm::vec3 ParticleEmitter::getInitialVelocity() const
	{
		float interval = m_initialVelocityInterval.y - m_initialVelocityInterval.x;
		return glm::sphericalRand(1.f)* interval + m_initialVelocityInterval.x;
	}

	float ParticleEmitter::getInitialLifeTime() const
	{
		return glm::linearRand(m_lifeTimeInterval.x, m_lifeTimeInterval.y);
	}

	void ParticleEmitter::spawnParticles(int spawnCount)
	{
		if (spawnCount + m_aliveParticlesCount >= m_maxParticleCount)
			spawnCount = m_maxParticleCount - m_aliveParticlesCount;

		//buffer is too small, we can't add particles : 
		if (spawnCount <= 0)
			return;

		int previousParticleCount = m_aliveParticlesCount;
		for (int i = previousParticleCount; i < previousParticleCount + spawnCount; i++) {
			m_positions[i]= m_translation;
			m_velocities[i] = getInitialVelocity();
			m_forces[i] = glm::vec3(0,0,0);
			m_elapsedTimes[i] = 0;
			m_lifeTimes[i] = getInitialLifeTime();
			m_colors[i] = m_colorSteps_values.size() > 0 ? m_colorSteps_values[0] : glm::vec4(1,1,1,1);
			m_sizes[i] = m_sizeSteps_values.size() > 0 ? m_sizeSteps_values[0] : glm::vec2(1,1);

			m_aliveParticlesCount++;
		}
	}

	void ParticleEmitter::update(float deltaTime)
	{
		float defaultParticleMass = 0.1f; //todo improve

		//spawn particles : 
		float particleCountToSpwan_float = m_particleCountBySecond * deltaTime + m_spawnFragment;
		float particleCountToSpwan_floored;
		m_spawnFragment = (float)modf(particleCountToSpwan_float, &particleCountToSpwan_floored);
		spawnParticles(particleCountToSpwan_floored);

		//update particles : 
		assert((m_aliveParticlesCount <= m_maxParticleCount));
		for (int i = 0; i < m_aliveParticlesCount; i++)
		{
			assert(m_maxParticleCount == m_elapsedTimes.size());
			m_elapsedTimes[i] += deltaTime;

			assert(m_maxParticleCount == m_lifeTimes.size());
			if (m_elapsedTimes[i] > m_lifeTimes[i])
			{
				//kill the particle : 
				//if(m_aliveParticlesCount<m_maxParticleCount && m_aliveParticlesCount != 0)
				swapParticles(i, m_aliveParticlesCount-1);
				m_aliveParticlesCount--;
				i--;
				continue;
			}
			else
			{
				assert(m_maxParticleCount == m_forces.size());
				assert(m_maxParticleCount == m_velocities.size());
				assert(m_maxParticleCount == m_positions.size());

				//give some forces to the particle : 
				m_forces[i] += getInternalParticleForce(m_elapsedTimes[i], m_lifeTimes[i], m_positions[i]);

				//compute forces applied to the point :
				m_velocities[i] += (deltaTime / defaultParticleMass)*m_forces[i];
				m_positions[i] += deltaTime*m_velocities[i];
				m_forces[i] = glm::vec3(0, 0, 0);

				//compute shape and colors : 
				assert(m_maxParticleCount == m_colors.size());
				m_colors[i] = getInternalParticleColor(m_elapsedTimes[i], m_lifeTimes[i], m_positions[i]);
				assert(m_maxParticleCount == m_sizes.size());
				m_sizes[i] = getInternalParticleSize(m_elapsedTimes[i], m_lifeTimes[i], m_positions[i]);
			}
		}

		updateVbos();
	}


	void ParticleEmitter::render(const glm::mat4 & projection, const glm::mat4 & view)
	{
		glm::mat4 VP = projection * view;
		glm::vec3 CameraRight = glm::vec3(view[0][0], view[1][0], view[2][0]);
		glm::vec3 CameraUp = glm::vec3(view[0][1], view[1][1], view[2][1]);

		m_materialParticules->use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_particleTexture->glId);

		m_materialParticules->glUniform_VP(VP);
		m_materialParticules->setUniformCameraRight(CameraRight);
		m_materialParticules->setUniformCameraUp(CameraUp);
		m_materialParticules->setUniformTexture(0);

		draw();
	}

	void ParticleEmitter::draw()
	{
		glBindVertexArray(m_vao);


		glVertexAttribDivisor(VERTICES, 0);
		glVertexAttribDivisor(NORMALS, 0);
		glVertexAttribDivisor(UVS, 0);
		glVertexAttribDivisor(POSITIONS, 1);
		glVertexAttribDivisor(COLORS, 1);
		glVertexAttribDivisor(SIZES, 1);

		glDrawElementsInstanced(GL_TRIANGLES, m_triangleCount * 3, GL_UNSIGNED_INT, (GLvoid*)0, m_aliveParticlesCount);

		glBindVertexArray(0);

	}

	void ParticleEmitter::updateVbos()
	{
		//instanced stuff :
		glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_aliveParticlesCount*sizeof(glm::vec3), &m_positions[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_aliveParticlesCount*sizeof(glm::vec4), &m_colors[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboSizes);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_aliveParticlesCount*sizeof(glm::vec2), &m_sizes[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void ParticleEmitter::onChangeMaxParticleCount()
	{
		if (m_maxParticleCount <= 0)
			m_maxParticleCount = 1;

		if (m_maxParticleCount <= m_aliveParticlesCount)
			m_aliveParticlesCount = m_maxParticleCount;

		m_positions.resize(m_maxParticleCount);
		m_velocities.resize(m_maxParticleCount);
		m_forces.resize(m_maxParticleCount);
		m_elapsedTimes.resize(m_maxParticleCount);
		m_lifeTimes.resize(m_maxParticleCount);
		m_colors.resize(m_maxParticleCount);
		m_sizes.resize(m_maxParticleCount);

		//instanced stuff :
		glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
		glEnableVertexAttribArray(POSITIONS);
		glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), &m_positions[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
		glEnableVertexAttribArray(COLORS);
		glBufferData(GL_ARRAY_BUFFER, m_colors.size()*sizeof(glm::vec4), &m_colors[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboSizes);
		glEnableVertexAttribArray(SIZES);
		glBufferData(GL_ARRAY_BUFFER, m_sizes.size()*sizeof(glm::vec2), &m_sizes[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void ParticleEmitter::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
	{
		m_translation = translation;
		m_rotation = rotation;
	}

	void ParticleEmitter::drawUI(Scene& scene)
	{
		
		//particle texture : 
		char texName[20];
		int stringLength = std::min((int)m_particleTextureName.size(), 20);
		m_particleTextureName.copy(texName, stringLength);
		texName[stringLength] = '\0';
		if (ImGui::InputText("textureName", texName, 20))
		{
			m_particleTextureName = texName;

			if (TextureFactory::get().contains(m_particleTextureName))
			{
				m_particleTexture = TextureFactory::get().get(m_particleTextureName);
			}
		}

		//size step :
		ImGui::PushID("SizeSteps");
		if (ImGui::Button("add size step")) {
			m_sizeSteps_times.push_back(1.f);
			m_sizeSteps_values.push_back(glm::vec2(1, 1));
		}
		for (int i = 0; i < m_sizeSteps_times.size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::InputFloat("##sizeStepTime", &m_sizeSteps_times[i])) {
				if (m_sizeSteps_times[i] < 0) m_sizeSteps_times[i] = 0;
				else if (m_sizeSteps_times[i] > 1.f) m_sizeSteps_times[i] = 1.f;
			}
			ImGui::SameLine();
			ImGui::InputFloat2("##sizeStepValue", &m_sizeSteps_values[i][0]);
			ImGui::SameLine();
			if (ImGui::Button("remove")) {
				m_sizeSteps_times.erase(m_sizeSteps_times.begin() + i);
				m_sizeSteps_values.erase(m_sizeSteps_values.begin() + i);
				i--;
			}
			ImGui::PopID();
		}
		ImGui::PopID();

		//color step :
		ImGui::PushID("ColorSteps");
		if (ImGui::Button("add color step")) {
			m_colorSteps_times.push_back(1.f);
			m_colorSteps_values.push_back(glm::vec4(1, 1, 1, 1));
		}
		for (int i = 0; i < m_colorSteps_times.size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::InputFloat("##colorStepTime", &m_colorSteps_times[i])) {
				if (m_colorSteps_times[i] < 0) m_colorSteps_times[i] = 0;
				else if (m_colorSteps_times[i] > 1.f) m_colorSteps_times[i] = 1.f;
			}
			ImGui::SameLine();
			ImGui::ColorEdit4("##colorStepValue", &m_colorSteps_values[i][0]);
			ImGui::SameLine();
			if (ImGui::Button("remove")) {
				m_colorSteps_times.erase(m_colorSteps_times.begin() + i);
				m_colorSteps_values.erase(m_colorSteps_values.begin() + i);
				i--;
			}
			ImGui::PopID();
		}
		ImGui::PopID();

		//force step :
		ImGui::PushID("ForceSteps");
		if (ImGui::Button("add force step")) {
			m_forceSteps_times.push_back(1.f);
			m_forceSteps_values.push_back(glm::vec3(0, 0, 0));
		}
		for (int i = 0; i < m_forceSteps_times.size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::InputFloat("##colorStepTime", &m_forceSteps_times[i])) {
				if (m_forceSteps_times[i] < 0) m_forceSteps_times[i] = 0;
				else if (m_forceSteps_times[i] > 1.f) m_forceSteps_times[i] = 1.f;
			}
			ImGui::SameLine();
			ImGui::InputFloat3("##forceStepValue", &m_forceSteps_values[i][0]);
			ImGui::SameLine();
			if (ImGui::Button("remove")) {
				m_forceSteps_times.erase(m_forceSteps_times.begin() + i);
				m_forceSteps_values.erase(m_forceSteps_values.begin() + i);
				i--;
			}
			ImGui::PopID();
		}
		ImGui::PopID();

		//particle by second :
		ImGui::InputFloat("particle count by second", &m_particleCountBySecond);

		//life time interval :
		ImGui::InputFloat2("life time interval", &m_lifeTimeInterval[0]);

		//initial velocity interval :
		ImGui::InputFloat2("initial velocity interval", &m_initialVelocityInterval[0]);

		//max particle count : 
		if (ImGui::InputInt("max particle count", &m_maxParticleCount)) {
			onChangeMaxParticleCount();
		}

	}

	void ParticleEmitter::eraseFromScene(Scene& scene)
	{
		scene.erase(this);
	}

	void ParticleEmitter::addToScene(Scene& scene)
	{
		scene.add(this);
	}

	Component * ParticleEmitter::clone(Entity* entity)
	{
		ParticleEmitter* newParticleEmitter = new ParticleEmitter(*this);

		newParticleEmitter->attachToEntity(entity);

		return newParticleEmitter;
	}

	void ParticleEmitter::addToEntity(Entity& entity)
	{
		entity.add(this);
	}

	void ParticleEmitter::eraseFromEntity(Entity& entity)
	{
		entity.erase(this);
	}

	void ParticleEmitter::save(Json::Value & rootComponent) const
	{
		//TODO
	}

	void ParticleEmitter::load(Json::Value & rootComponent)
	{
		//TODO
	}

}

////positions : 
//glGenBuffers(1, &m_vboPositionsA);
//glBindBuffer(GL_ARRAY_BUFFER, m_vboPositionsA);
//glEnableVertexAttribArray(POSITIONS);
//glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), &m_positions[0], GL_STREAM_DRAW);
//glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

////velocities : 
//glGenBuffers(1, &m_vboVelocitiesA);
//glBindBuffer(GL_ARRAY_BUFFER, m_vboVelocitiesA);
//glEnableVertexAttribArray(VELOCITIES);
//glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), &m_velocities[0], GL_STREAM_DRAW);
//glVertexAttribPointer(VELOCITIES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);


////forces : 
//glGenBuffers(1, &m_vboForcesA);
//glBindBuffer(GL_ARRAY_BUFFER, m_vboForcesA);
//glEnableVertexAttribArray(FORCES);
//glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), &m_forces[0], GL_STREAM_DRAW);
//glVertexAttribPointer(FORCES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);


////elapsed times : 
//glGenBuffers(1, &m_vboElapsedTimesA);
//glBindBuffer(GL_ARRAY_BUFFER, m_vboElapsedTimesA);
//glEnableVertexAttribArray(ELAPSED_TIMES);
//glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), &m_elapsedTimes[0], GL_STREAM_DRAW);
//glVertexAttribPointer(ELAPSED_TIMES, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 1, (void*)0);

////life times : 
//glGenBuffers(1, &m_vboLifeTimesA);
//glBindBuffer(GL_ARRAY_BUFFER, m_vboLifeTimesA);
//glEnableVertexAttribArray(LIFE_TIMES);
//glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), &m_lifeTimes[0], GL_STREAM_DRAW);
//glVertexAttribPointer(LIFE_TIMES, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 1, (void*)0);

////colors : 
//glGenBuffers(1, &m_vboColorsA);
//glBindBuffer(GL_ARRAY_BUFFER, m_vboColorsA);
//glEnableVertexAttribArray(COLORS);
//glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), &m_colors[0], GL_STREAM_DRAW);
//glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);

////sizes : 
//glGenBuffers(1, &m_vboSizesA);
//glBindBuffer(GL_ARRAY_BUFFER, m_vboSizesA);
//glEnableVertexAttribArray(SIZES);
//glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), &m_sizes[0], GL_STREAM_DRAW);
//glVertexAttribPointer(SIZES, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);

//glBindVertexArray(0);
//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//glBindBuffer(GL_ARRAY_BUFFER, 0);

////out buffers : 

//glGenBuffers(1, &m_vboPositionsB);
//glBindBuffer(GL_ARRAY_BUFFER, m_vboPositionsB);
//glEnableVertexAttribArray(POSITIONS);
//glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), nullptr, GL_STREAM_DRAW);
//glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

//glGenBuffers(1, &m_vboVelocitiesB);
//glBindBuffer(GL_ARRAY_BUFFER, m_vboVelocitiesB);
//glEnableVertexAttribArray(VELOCITIES);
//glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), nullptr, GL_STREAM_DRAW);
//glVertexAttribPointer(VELOCITIES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);


//glGenBuffers(1, &m_vboForcesB);
//glBindBuffer(GL_ARRAY_BUFFER, m_vboForcesB);
//glEnableVertexAttribArray(FORCES);
//glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), nullptr, GL_STREAM_DRAW);
//glVertexAttribPointer(FORCES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);


/*glGenBuffers(1, &m_vboElapsedTimesB);
glBindBuffer(GL_ARRAY_BUFFER, m_vboElapsedTimesB);
glEnableVertexAttribArray(ELAPSED_TIMES);
glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), nullptr, GL_STREAM_DRAW);
glVertexAttribPointer(ELAPSED_TIMES, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 1, (void*)0);

glGenBuffers(1, &m_vboLifeTimesB);
glBindBuffer(GL_ARRAY_BUFFER, m_vboLifeTimesB);
glEnableVertexAttribArray(LIFE_TIMES);
glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), nullptr, GL_STREAM_DRAW);
glVertexAttribPointer(LIFE_TIMES, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 1, (void*)0);

glGenBuffers(1, &m_vboColorsB);
glBindBuffer(GL_ARRAY_BUFFER, m_vboColorsB);
glEnableVertexAttribArray(COLORS);
glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), nullptr, GL_STREAM_DRAW);
glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);

glGenBuffers(1, &m_vboSizesB);
glBindBuffer(GL_ARRAY_BUFFER, m_vboSizesB);
glEnableVertexAttribArray(SIZES);
glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), nullptr, GL_STREAM_DRAW);
glVertexAttribPointer(SIZES, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);*/

///////////////////////////

//glBindVertexArray(m_vao);
/*

//positions :
glEnableVertexAttribArray(POSITIONS);
glBindBuffer(GL_ARRAY_BUFFER, m_vboPositionsA);
glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
//velocities :
glEnableVertexAttribArray(VELOCITIES);
glBindBuffer(GL_ARRAY_BUFFER, m_vboVelocitiesA);
glVertexAttribPointer(VELOCITIES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
//forces :
glEnableVertexAttribArray(FORCES);
glBindBuffer(GL_ARRAY_BUFFER, m_vboForcesA);
glVertexAttribPointer(FORCES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
//elapsed times :
glEnableVertexAttribArray(ELAPSED_TIMES);
glBindBuffer(GL_ARRAY_BUFFER, m_vboElapsedTimesA);
glVertexAttribPointer(ELAPSED_TIMES, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 1, (void*)0);
//life times :
glEnableVertexAttribArray(LIFE_TIMES);
glBindBuffer(GL_ARRAY_BUFFER, m_vboLifeTimesA);
glVertexAttribPointer(LIFE_TIMES, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 1, (void*)0);
//colors :
glEnableVertexAttribArray(COLORS);
glBindBuffer(GL_ARRAY_BUFFER, m_vboColorsA);
glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
//sizes :
glEnableVertexAttribArray(SIZES);
glBindBuffer(GL_ARRAY_BUFFER, m_vboSizesA);
glVertexAttribPointer(SIZES, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);

*/

//glVertexAttribDivisor(VERTICES, 0);
//glVertexAttribDivisor(NORMALS, 0);
//glVertexAttribDivisor(UVS, 0);
//glVertexAttribDivisor(POSITIONS, 1);
//glVertexAttribDivisor(VELOCITIES, 1);
//glVertexAttribDivisor(FORCES, 1);
//glVertexAttribDivisor(ELAPSED_TIMES, 1);
//glVertexAttribDivisor(LIFE_TIMES, 1);
//glVertexAttribDivisor(COLORS, 1);
//glVertexAttribDivisor(SIZES, 1);

//glDrawElementsInstanced(GL_TRIANGLES, m_triangleCount * 3, GL_UNSIGNED_INT, (GLvoid*)0, m_maxParticleCount);

//glBindBuffer(GL_ARRAY_BUFFER, 0);
//glBindVertexArray(0);

/////////////////////////


////positions : 
//glBindBuffer(GL_ARRAY_BUFFER, m_vboPositionsA);
//glEnableVertexAttribArray(POSITIONS);
//glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
////velocities :
//glBindBuffer(GL_ARRAY_BUFFER, m_vboVelocitiesA);
//glEnableVertexAttribArray(VELOCITIES);
//glVertexAttribPointer(VELOCITIES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
////forces :
//glBindBuffer(GL_ARRAY_BUFFER, m_vboForcesA);
//glEnableVertexAttribArray(FORCES);
//glVertexAttribPointer(FORCES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

////elapsed times : 
//glBindBuffer(GL_ARRAY_BUFFER, m_vboElapsedTimesA);
//glEnableVertexAttribArray(ELAPSED_TIMES);
//glVertexAttribPointer(ELAPSED_TIMES, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 1, (void*)0);
////life times : 
//glBindBuffer(GL_ARRAY_BUFFER, m_vboLifeTimesA);
//glEnableVertexAttribArray(LIFE_TIMES);
//glVertexAttribPointer(LIFE_TIMES, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 1, (void*)0);
////colors : 
//glBindBuffer(GL_ARRAY_BUFFER, m_vboColorsA);
//glEnableVertexAttribArray(COLORS);
//glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
////sizes : 
//glBindBuffer(GL_ARRAY_BUFFER, m_vboSizesA);
//glEnableVertexAttribArray(SIZES);
//glVertexAttribPointer(SIZES, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);

//bind output buffers
//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vboPositionsB);
//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, m_vboVelocitiesB);
//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, m_vboForcesB);
/*glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 3, m_vboElapsedTimesB);
glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 4, m_vboLifeTimesB);
glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 5, m_vboColorsB);
glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 6, m_vboSizesB);*/

//launch kernel
//glBeginTransformFeedback(GL_POINTS);
//glDrawArrays(GL_POINTS, 0, m_maxParticleCount);
//glEndTransformFeedback();
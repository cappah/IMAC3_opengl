#include "ParticleEmitter.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

namespace Physic {


	ParticleEmitter::ParticleEmitter() : Component(PARTICLE_EMITTER), m_maxParticleCount(10),
	m_materialParticules(MaterialFactory::get().get<MaterialParticles>("defaultParticles")), m_materialParticuleSimulation(MaterialFactory::get().get<MaterialParticleSimulation>("particleSimulation")),
	m_triangleIndex({ 0, 1, 2, 2, 3, 0 }),
	m_uvs({ 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f }), m_vertices({ -0.5, 0.0, -0.5, 0.5, 0.0, -0.5, 0.5, 0.0, 0.5, -0.5, 0.0, 0.5 }), 
	m_normals({ 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 }), m_particleTexture(TextureFactory::get().get("default"))
	{
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

		//init model :
		m_triangleIndex = { 0, 1, 2, 2, 3, 0 };
		m_uvs = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
		m_vertices = { -0.5, 0.0, -0.5, 0.5, 0.0, -0.5, 0.5, 0.0, 0.5, -0.5, 0.0, 0.5 };
		m_normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
		
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
		glEnableVertexAttribArray(VERTICES);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(float), &m_vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(VERTICES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

		glGenBuffers(1, &m_vboNormals);
		glEnableVertexAttribArray(NORMALS);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(float), &m_normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);


		glGenBuffers(1, &m_vboUvs);
		glEnableVertexAttribArray(UVS);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboUvs);
		glBufferData(GL_ARRAY_BUFFER, m_uvs.size()*sizeof(float), &m_uvs[0], GL_STATIC_DRAW);
		glVertexAttribPointer(UVS, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Instanced stuff : 
		//positions : 
		glGenBuffers(1, &m_vboPositionsA);
		glEnableVertexAttribArray(POSITIONS);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboPositionsA);
		glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), &m_positions[0], GL_STREAM_DRAW);
		glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
		
		glGenBuffers(1, &m_vboPositionsB);
		glEnableVertexAttribArray(POSITIONS);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboPositionsB);
		glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), nullptr, GL_STREAM_DRAW);
		glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

		//velocities : 
		glGenBuffers(1, &m_vboVelocitiesA);
		glEnableVertexAttribArray(VELOCITIES);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboVelocitiesA);
		glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), &m_velocities[0], GL_STREAM_DRAW);
		glVertexAttribPointer(VELOCITIES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

		glGenBuffers(1, &m_vboVelocitiesB);
		glEnableVertexAttribArray(VELOCITIES);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboVelocitiesB);
		glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), nullptr, GL_STREAM_DRAW);
		glVertexAttribPointer(VELOCITIES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

		//forces : 
		glGenBuffers(1, &m_vboForcesA);
		glEnableVertexAttribArray(FORCES);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboForcesA);
		glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), &m_forces[0], GL_STREAM_DRAW);
		glVertexAttribPointer(FORCES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

		glGenBuffers(1, &m_vboForcesB);
		glEnableVertexAttribArray(FORCES);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboForcesB);
		glBufferData(GL_ARRAY_BUFFER, m_maxParticleCount*sizeof(float), nullptr, GL_STREAM_DRAW);
		glVertexAttribPointer(FORCES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

		//TODO ...

	}

	void ParticleEmitter::draw()
	{
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboPositionsA);
		glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

		glVertexAttribDivisor(POSITIONS, 1);

		glDrawElementsInstanced(GL_TRIANGLES, m_triangleCount * 3, GL_UNSIGNED_INT, (GLvoid*)0, m_maxParticleCount);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void ParticleEmitter::simulateOnGPU(float deltaTime)
	{
		m_materialParticuleSimulation->use();
		m_materialParticuleSimulation->glUniform_deltaTime(deltaTime);


		glEnable(GL_RASTERIZER_DISCARD);	

		glEnableVertexAttribArray(POSITIONS);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboPositionsA);
		glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);


		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vboPositionsB);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, m_vboVelocitiesB);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, m_vboForcesB);
		//...

		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, m_maxParticleCount);
		glEndTransformFeedback();

		std::swap(m_vboPositionsA, m_vboPositionsB);
		std::swap(m_vboVelocitiesA, m_vboVelocitiesB);
		std::swap(m_vboForcesA, m_vboForcesB);
		//...

		glDisable(GL_RASTERIZER_DISCARD);
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

	void ParticleEmitter::drawUI(Scene& scene)
	{
		//TODO


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

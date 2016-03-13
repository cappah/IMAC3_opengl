#include "ParticleEmitter.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

namespace Physic {


	ParticleEmitter::ParticleEmitter() : Component(PARTICLE_EMITTER), m_triangleIndex({ 0, 1, 2, 2, 3, 0 }),
	m_uvs({ 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f }), m_vertices({ -0.5, 0.0, -0.5, 0.5, 0.0, -0.5, 0.5, 0.0, 0.5, -0.5, 0.0, 0.5 }), 
	m_normals({ 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 })
	{
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

		//Instanced stuff : 
		glGenBuffers(1, &m_vboUvs);
		glEnableVertexAttribArray(POSITIONS);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
		glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(float), &m_positions[0], GL_STREAM_DRAW);
		glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

		//TODO ...



		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

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

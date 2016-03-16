#include "ParticleEmitter.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

namespace Physic {


	ParticleEmitter::ParticleEmitter() : Component(PARTICLE_EMITTER), m_maxParticleCount(10),
	m_materialParticules(MaterialFactory::get().get<MaterialParticles>("particles")),
	m_materialParticuleSimulation(MaterialFactory::get().get<MaterialParticleSimulation>("particleSimulation")),
	m_triangleIndex({ 0, 1, 2, 2, 3, 0 }),
	m_uvs({ 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f }), m_vertices({ -0.5, 0.0, -0.5, 0.5, 0.0, -0.5, 0.5, 0.0, 0.5, -0.5, 0.0, 0.5 }), 
	m_normals({ 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 }), m_particleTexture(TextureFactory::get().get("default")),
	m_currVB(0), m_currTFB(1), m_isFirst(true)
	{
		////initialize system : 
		//for (int i = 0; i < m_maxParticleCount; i++)
		//{
		//	m_positions.push_back(glm::vec3(0, 0, 0));
		//	m_velocities.push_back(glm::vec3(0,0,0));
		//	m_forces.push_back(glm::vec3(0,0,0));
		//	m_elapsedTimes.push_back(0.f);
		//	m_lifeTimes.push_back(5.f);
		//	m_colors.push_back(glm::vec4(1,0,0,1));
		//	m_sizes.push_back(glm::vec2(1.f, 1.f));
		//}

		m_particles.resize(m_maxParticleCount);

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
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//Instanced stuff : 
		glGenTransformFeedbacks(2, m_transformFeedback);
		glGenBuffers(2, m_particleBuffer);

		for (unsigned int i = 0; i < 2; i++) {
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
			glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Particle)*m_maxParticleCount, &m_particles[0], GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
		}

	}

	void ParticleEmitter::simulateOnGPU(float deltaTime)
	{
		int tmp = m_currVB;
		m_currVB = m_currTFB;
		m_currTFB = tmp;

		m_materialParticuleSimulation->use();
		m_materialParticuleSimulation->glUniform_deltaTime(deltaTime);

		glEnable(GL_RASTERIZER_DISCARD);

		glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);

		glEnableVertexAttribArray(POSITIONS);
		glEnableVertexAttribArray(VELOCITIES);
		glEnableVertexAttribArray(FORCES);
		glEnableVertexAttribArray(ELAPSED_TIMES);
		glEnableVertexAttribArray(LIFE_TIMES);
		glEnableVertexAttribArray(SIZES);
		glEnableVertexAttribArray(COLORS);

		glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Physic::Particle, position));
		glVertexAttribPointer(VELOCITIES, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Physic::Particle, velocity));
		glVertexAttribPointer(FORCES, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Physic::Particle, force));
		glVertexAttribPointer(ELAPSED_TIMES, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Physic::Particle, elapsedTime));
		glVertexAttribPointer(LIFE_TIMES, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Physic::Particle, lifeTime));
		glVertexAttribPointer(SIZES, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Physic::Particle, size));
		glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Physic::Particle, color)); 


		glBeginTransformFeedback(GL_POINTS);
		if (m_isFirst) {
			glDrawArrays(GL_POINTS, 0, 1);
			m_isFirst = false;
		}
		else {
			glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]);
		}
		glEndTransformFeedback();

		glDisableVertexAttribArray(POSITIONS);
		glDisableVertexAttribArray(VELOCITIES);
		glDisableVertexAttribArray(FORCES);
		glDisableVertexAttribArray(ELAPSED_TIMES);
		glDisableVertexAttribArray(LIFE_TIMES);
		glDisableVertexAttribArray(SIZES);
		glDisableVertexAttribArray(COLORS);

		//swap vbos for ping-pong efect (we only use two vbo for each parameter)
		//std::swap(m_vboPositionsA, m_vboPositionsB);
		//std::swap(m_vboVelocitiesA, m_vboVelocitiesB);
		//std::swap(m_vboForcesA, m_vboForcesB);
		//std::swap(m_vboElapsedTimesA, m_vboElapsedTimesB);
		//std::swap(m_vboLifeTimesA, m_vboLifeTimesB);
		//std::swap(m_vboColorsA, m_vboColorsB);
		//std::swap(m_vboSizesA, m_vboSizesB);

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

	void ParticleEmitter::draw()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);
		glEnableVertexAttribArray(POSITIONS);
		glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Physic::Particle, position)); // position

		glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);

		GLfloat feedback[10];
		glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(feedback), feedback);
		for (int i = 0; i < 10; i++) {
			std::cout << "pos = " << feedback[i] << std::endl;
		}

		glDisableVertexAttribArray(0);

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
#pragma once

#include "Mesh.h"
#include "Materials.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Component.h"
#include "Materials.h"

namespace Physic{

	struct Particle
	{
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 force;
		float elapsedTime;
		float lifeTime;
		glm::vec4 color;
		glm::vec2 size;
	};

	class ParticleEmitter : public Component
	{
	public :
		enum VBO_TYPES {VERTICES = 0, NORMALS, UVS,
			POSITIONS, VELOCITIES, FORCES, ELAPSED_TIMES, LIFE_TIMES, COLORS, SIZES};
	private:
		//parameters : 
		int m_maxParticleCount;
		std::vector<glm::vec2> m_sizeSteps;
		std::vector<glm::vec4> m_colorSteps;
		glm::vec2 m_lifeTimeInterval;
		Texture* m_particleTexture;

		//particles soa : 
		//std::vector<glm::vec3> m_positions;
		//std::vector<glm::vec3> m_velocities;
		//std::vector<glm::vec3> m_forces;
		//std::vector<float> m_elapsedTimes;
		//std::vector<float> m_lifeTimes;
		//std::vector<glm::vec4> m_colors;
		//std::vector<glm::vec2> m_sizes;
		std::vector<Particle> m_particles;

		//model :
		int m_triangleCount;
		std::vector<unsigned int> m_triangleIndex;
		std::vector<float> m_uvs;
		std::vector<float> m_vertices;
		std::vector<float> m_normals;

		//materials : 
		MaterialParticles* m_materialParticules;
		MaterialParticleSimulation* m_materialParticuleSimulation;

		//opengl stuff : 
		GLuint m_vao;
		GLuint m_index;
		GLuint m_vboUvs;
		GLuint m_vboVertices;
		GLuint m_vboNormals;
		//instanced infos : 
		/*GLuint m_vboPositionsA;
		GLuint m_vboPositionsB;
		GLuint m_vboVelocitiesA;
		GLuint m_vboVelocitiesB;
		GLuint m_vboForcesA;
		GLuint m_vboForcesB;
		GLuint m_vboLifeTimesA;
		GLuint m_vboLifeTimesB;
		GLuint m_vboElapsedTimesA;
		GLuint m_vboElapsedTimesB;
		GLuint m_vboColorsA;
		GLuint m_vboColorsB;
		GLuint m_vboSizesA;
		GLuint m_vboSizesB;*/

		bool m_isFirst;
		unsigned int m_currVB;
		unsigned int m_currTFB;
		GLuint m_particleBuffer[2];
		GLuint m_transformFeedback[2];

	public:
		ParticleEmitter();
		~ParticleEmitter();
		void initGl();
		void draw();
		void simulateOnGPU(float deltaTime);
		void render(const glm::mat4& projection, const glm::mat4& view);

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


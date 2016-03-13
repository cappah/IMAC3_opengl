#pragma once

#include "Mesh.h"
#include "Materials.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Component.h"

namespace Physic{

	class ParticleEmitter : public Component
	{
	public :
		enum VBO_TYPES {VERTICES, NORMALS, UVS, POSITIONS, VELOCITIES, FORCES, ELAPSED_TIMES, LIFE_TIMES, COLORS};
	private:
		//parameters : 
		int m_maxParticleCount;
		std::vector<glm::vec2> m_sizeSteps;
		std::vector<glm::vec4> m_colorSteps;
		glm::vec2 m_lifeTimeInterval;

		//particles soa : 
		std::vector<float> m_elapsedTimes;
		std::vector<float> m_lifeTimes;
		std::vector<glm::vec4> m_colors;
		std::vector<glm::vec2> m_sizes;
		std::vector<glm::vec3> m_positions;
		std::vector<glm::vec3> m_velocities;
		std::vector<glm::vec3> m_forces;

		//model :
		int m_triangleCount;
		std::vector<unsigned int> m_triangleIndex;
		std::vector<float> m_uvs;
		std::vector<float> m_vertices;
		std::vector<float> m_normals;

		//opengl stuff : 
		GLuint m_vao;
		GLuint m_index;
		GLuint m_vboUvs;
		GLuint m_vboVertices;
		GLuint m_vboNormals;
		//instanced infos : 
		GLuint m_vboLifeTimes;
		GLuint m_vboElapsedTimes;
		GLuint m_vboColors;
		GLuint m_vboSizes;
		GLuint m_vboPositions;
		GLuint m_vbovelocities;
		GLuint m_vboForces;

	public:
		ParticleEmitter();
		~ParticleEmitter();
		void initGl();

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


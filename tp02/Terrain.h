#pragma once

#include <vector>

#include "glew/glew.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Materials.h"
#include "PerlinNoise.h"

class Terrain
{
private:
	enum Vbo_types { VERTICES = 0, NORMALS, UVS };

	int m_triangleCount;

	std::vector<int> m_triangleIndex;
	std::vector<float> m_uvs;
	std::vector<float> m_vertices;
	std::vector<float> m_normals;

	std::vector<float> m_heightMap;

	GLuint vbo_index;
	GLuint vbo_vertices;
	GLuint vbo_uvs;
	GLuint vbo_normals;
	GLuint vao;

	int m_subdivision;

	MaterialLit m_material;

	float m_width;
	float m_depth;
	float m_height;

	glm::vec3 m_offset;

	std::string diffuseTextureName;
	std::string specularTextureName;

	NoiseGenerator m_terrainNoise;

public:

	Terrain(float width = 100, float height = 30, float depth = 100, int subdivision = 10, glm::vec3 offset = glm::vec3(0,0,0));
	~Terrain();
	//initialize vbos and vao, based on the informations of the mesh.
	void initGl();

	// simply draw the vertices, using vao.
	void render(const glm::mat4& projection, const glm::mat4& view);

	void drawUI();

	void computeNormals();

	void applyNoise(Perlin2D& perlin2D);
	void updateTerrain();
	void generateTerrain();
};


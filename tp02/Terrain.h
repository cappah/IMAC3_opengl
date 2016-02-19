#pragma once

#include <vector>

#include "glew/glew.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Mesh.h"
#include "Materials.h"
#include "PerlinNoise.h"

class Terrain
{
private:
	enum Vbo_types { VERTICES = 0, NORMALS, UVS, TANGENTS };

	int m_triangleCount;

	std::vector<int> m_triangleIndex;
	std::vector<float> m_uvs;
	std::vector<float> m_vertices;
	std::vector<float> m_normals;
	std::vector<float> m_tangents;

	std::vector<float> m_heightMap;

	GLuint vbo_index;
	GLuint vbo_vertices;
	GLuint vbo_uvs;
	GLuint vbo_normals;
	GLuint vbo_tangents;
	GLuint vao;

	int m_subdivision;

	MaterialTerrain m_material;

	float m_width;
	float m_depth;
	float m_height;

	glm::vec3 m_offset;

	int m_seed;
	NoiseGenerator m_terrainNoise;

	//texture tool : 
	Texture m_noiseTexture;

	MaterialDrawOnTexture m_drawOnTextureMaterial;
	MaterialTerrainEdition m_terrainMaterial;

	Mesh m_quadMesh;

	GLuint m_terrainFbo;
	GLuint m_materialLayoutsFBO;

	Texture m_filterTexture;
	Texture m_terrainDiffuse;
	Texture m_terrainBump;
	Texture m_terrainSpecular;
	Texture m_drawMatTexture;
	//Texture m_terrainDepth;

	std::vector<MaterialLit*> m_terrainLayouts;
	std::vector<glm::vec2> m_textureRepetitions;

	float m_noiseMax;
	float m_noiseMin;

	//for UI : 
	char m_newLayoutName[30];


public:

	Terrain(float width = 100, float height = 30, float depth = 100, int subdivision = 10, glm::vec3 offset = glm::vec3(0,0,0));
	~Terrain();
	//initialize vbos and vao, based on the informations of the mesh.
	void initGl();

	// simply draw the vertices, using vao.
	void render(const glm::mat4& projection, const glm::mat4& view);

	void drawUI();

	void computeNormals();

	void applyNoise(Perlin2D& perlin2D, bool _computeNoiseTexture = true);
	void updateTerrain();
	void generateTerrain();

	void computeNoiseTexture(Perlin2D& perlin2D);
	void generateTerrainTexture();

	void drawMaterialOnTerrain(glm::vec3 position, float radius, int textureIdx);
};


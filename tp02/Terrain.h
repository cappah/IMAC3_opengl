#pragma once

#include <vector>

#include "glew/glew.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Mesh.h"
#include "Materials.h"
#include "PerlinNoise.h"

#include "Point.h"
#include "Link.h"
#include "WindZone.h"

//forwards : 
class Ray;

struct GrassKey
{
	int i;
	int j;

	inline GrassKey(int _i, int _j) : i(_i), j(_j)
	{}

	inline bool operator==(const GrassKey& other)
	{
		return i == other.i && j == other.j;
	}
};

//structure which store infos to render grass in instanced mode
struct GrassField {

	enum VboTypes {VERTICES = 0, NORMALS, UVS, POSITIONS, ANIM_POS};
	
	MaterialGrassField materialGrassField;

	Texture* grassTexture;

	GLuint vao;
	
	int triangleCount;

	std::vector<int> triangleIndex;
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<float> positions; //grass positions
	std::vector<GrassKey> grassKeys; //keys to identity grass
	//for physic simulation : 
	std::vector<Physic::Point> physicPoints;
	std::vector<Physic::Link> physicLinks;

	GLuint vbo_index;
	GLuint vbo_vertices;
	GLuint vbo_uvs;
	GLuint vbo_normals;
	//for physic simulation : 
	GLuint vbo_animPos;

	//additional vbos for instantiation : 
	GLuint vbo_pos;

	GrassField();
	~GrassField();
	void initGl();

	void addGrass(GrassKey grassKey, const glm::vec3& position);
	void remove(GrassKey grassKey);

	//draw all grass with instantiation : 
	void draw();

	//render all grass with instantiation : 
	void render(const glm::mat4& projection, const glm::mat4& view);

	//update physic : 
	void updatePhysic(std::vector<Physic::WindZone*>& windZones); //TODO

	void updateVBOPositions();
};


class Terrain
{
public : 
	enum TerrainTools { PARAMETER = 0, DRAW_MATERIAL, DRAW_GRASS, PERLIN };

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

	//grass Management :
	GrassField m_grassField;
	std::vector<int> m_grassLayout;
	float m_grassLayoutDelta; //the delta between two grass

	//for UI : 
	TerrainTools m_currentTerrainTool;
	char m_newLayoutName[30];
	int m_currentMaterialToDrawIdx;
	float m_drawRadius;
	int m_maxGrassDensity;
	int m_grassDensity;
	char m_newGrassTextureName[30];


public:

	Terrain(float width = 100, float height = 30, float depth = 100, int subdivision = 10, glm::vec3 offset = glm::vec3(0,0,0));
	~Terrain();
	//initialize vbos and vao, based on the informations of the mesh.
	void initGl();

	// simply draw the vertices, using vao.
	void render(const glm::mat4& projection, const glm::mat4& view);
	//a simple shortcut for this->m_grassField.render(projection, view)
	void renderGrassField(const glm::mat4& projection, const glm::mat4& view);

	void drawUI();

	void computeNormals();

	void applyNoise(Perlin2D& perlin2D, bool _computeNoiseTexture = true);
	void updateTerrain();
	void generateTerrain();

	void computeNoiseTexture(Perlin2D& perlin2D);
	void generateTerrainTexture();

	void drawMaterialOnTerrain(glm::vec3 position, float radius, int textureIdx);
	void drawMaterialOnTerrain(glm::vec3 position);

	bool isIntersectedByRay(const Ray& ray, CollisionInfo& collisionInfo) const;

	//get terrain height at a given point
	float getHeight(float x, float y);

	void drawGrassOnTerrain(const glm::vec3 position);
	void drawGrassOnTerrain(const glm::vec3 position, float radius, int density, int maxDensity);
	void updateGrassPositions();

	TerrainTools getCurrentTerrainTool() const;

	//update physic : 
	void updatePhysic(std::vector<Physic::WindZone*>& windZones);
};


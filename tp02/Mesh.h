#pragma once

#include <vector>
#include <iostream>

#include "glew/glew.h"

#include "glm/glm.hpp"
#include "glm/common.hpp"


#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct Mesh
{
	std::string name;

	glm::vec3 topRight;
	glm::vec3 bottomLeft;
	glm::vec3 origin;

	std::string path;

	enum Vbo_usage { USE_INDEX = 0x01, USE_VERTICES = 0x02, USE_UVS = 0x04, USE_NORMALS = 0x08 , USE_TANGENTS = 0x010};
	enum Vbo_types { VERTICES = 0, NORMALS, UVS, TANGENTS };

	int triangleCount;

	std::vector<int> triangleIndex;
	std::vector<float> uvs;
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> tangents;

	GLuint vbo_index;
	GLuint vbo_vertices;
	GLuint vbo_uvs;
	GLuint vbo_normals;
	GLuint vbo_tangents;
	GLuint vao;

	unsigned int vbo_usage;

	int coordCountByVertex;

	GLenum primitiveType;

	Mesh(GLenum _primitiveType = GL_TRIANGLES, unsigned int _vbo_usage = (USE_INDEX | USE_VERTICES | USE_UVS | USE_NORMALS), int _coordCountByVertex = 3);
	Mesh(const std::string& path);

	~Mesh();

	//initialize vbos and vao, based on the informations of the mesh.
	void initGl();

	// simply draw the vertices, using vao.
	void draw();

	void computeBoundingBox();

private:
	bool initFromScene(const aiScene* pScene, const std::string& Filename);
	void initMesh(unsigned int Index, const aiMesh* paiMesh);
};
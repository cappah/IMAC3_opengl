#pragma once

#include <vector>
#include <iostream>

#include "glew/glew.h"

#include "glm/glm.hpp"
#include "glm/common.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Skeleton.h"

//forwards : 
class Ray;
class CollisionInfo;

//struct SubMesh {
//
//	std::vector<int> triangleIndex;
//	std::vector<float> uvs;
//	std::vector<float> vertices;
//	std::vector<float> normals;
//	std::vector<float> tangents;
//
//	GLuint vbo_index;
//	GLuint vbo_vertices;
//	GLuint vbo_uvs;
//	GLuint vbo_normals;
//	GLuint vbo_tangents;
//	GLuint vao;
//
//};

struct Mesh
{
	std::string name;

	glm::vec3 topRight;
	glm::vec3 bottomLeft;
	glm::vec3 origin;

	std::string path;

	enum Vbo_usage { USE_INDEX = 1 << 0, USE_VERTICES = 1 << 1, USE_UVS = 1 << 2, USE_NORMALS = 1 << 3, USE_TANGENTS = 1 << 4 , USE_BONES = 1 << 5/* , USE_INSTANTIATION = 1 << 5 */};
	enum Vbo_types { VERTICES = 0, NORMALS, UVS, TANGENTS, INDEX, BONE_IDS, BONE_WEIGHTS /* INSTANCE_TRANSFORM */};

	int subMeshCount;
	int totalTriangleCount;
	std::vector<int> triangleCount;
	std::vector<GLuint> indexOffsets;
	Skeleton* skeleton;
	bool isSkeletalMesh; //true if the mesh owns a skeleton.

	std::vector<int> triangleIndex;
	std::vector<float> uvs;
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> tangents;
	
	//std::vector<glm::mat4> transforms;

	GLuint vbo_index;
	GLuint vbo_vertices;
	GLuint vbo_uvs;
	GLuint vbo_normals;
	GLuint vbo_tangents;
	GLuint vbo_bones;
	//GLuint vbo_transforms[4];
	GLuint vao;

	unsigned int vbo_usage;

	int coordCountByVertex;

	GLenum primitiveType;
	GLenum drawUsage;

	Mesh(GLenum _primitiveType = GL_TRIANGLES, unsigned int _vbo_usage = (USE_INDEX | USE_VERTICES | USE_UVS | USE_NORMALS), int _coordCountByVertex = 3, GLenum _drawUsage = GL_STATIC_DRAW);
	Mesh(const std::string& _path);

	~Mesh();

	//initialize vbos and vao, based on the informations of the mesh.
	void initGl();
	void freeGl();

	//update a single vbo.
	void updateVBO(Vbo_types type);
	//update all vbos.
	void updateAllVBOs();

	// simply draw the vertices, using vao.
	void draw();
	//draw a specific sub mesh.
	void draw(int idx);

	void computeBoundingBox();

	bool isIntersectedByRay(const Ray& ray, CollisionInfo& collisionInfo) const;

	Skeleton* getSkeleton() const;

private:
	bool initFromScene(const aiScene* pScene, const std::string& Filename);
	void initMesh(unsigned int Index, const aiMesh* paiMesh);
	//Check if the mesh has bones. If true, create the appropriate skeleton :  
	void loadBones(unsigned int meshIndex, const aiMesh * mesh, const aiNode * rootNode, unsigned int firstVertexId);
	void loadAnimations(const aiScene& scene);
};
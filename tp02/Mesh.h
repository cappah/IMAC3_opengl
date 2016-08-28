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
#include "Resource.h"

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

struct Mesh : public Resource
{
	Assimp::Importer* importer; //TODO : a quoi ça sert de laisser ça là ??? En fait ça doit servir pour gerer la durée de vie des animations, mais c'est pas cool...

	std::string name;

	glm::vec3 topRight;
	glm::vec3 bottomLeft;
	glm::vec3 origin;

	FileHandler::CompletePath path;

	enum Vbo_usage { USE_INDEX = 1 << 0, USE_VERTICES = 1 << 1, USE_UVS = 1 << 2, USE_NORMALS = 1 << 3, USE_TANGENTS = 1 << 4 , USE_BONES = 1 << 5/* , USE_INSTANTIATION = 1 << 5 */};
	enum Vbo_types { VERTICES = 0, NORMALS, UVS, TANGENTS, BONE_IDS, BONE_WEIGHTS /* INSTANCE_TRANSFORM */, INDEX };

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
	Mesh(const FileHandler::CompletePath& _path, const std::string& meshName = "");
	void init(const FileHandler::CompletePath& path) override;

	~Mesh();
	void clear();

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
	bool getIsSkeletalMesh() const;

private:
	bool initFromScene(const aiScene* pScene, const FileHandler::CompletePath& scenePath);
	void initMesh(unsigned int Index, const aiMesh* paiMesh);
	//Check if the mesh has bones. If true, create the appropriate skeleton :  
	void loadBones(unsigned int meshIndex, const aiMesh * mesh, const aiNode * rootNode, unsigned int firstVertexId);
	void loadAnimations(const FileHandler::CompletePath& scenePath, const aiScene* scene);
};
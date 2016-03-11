#include "Mesh.h"
#include "Ray.h"
#include "Utils.h"

Mesh::Mesh(GLenum _primitiveType , unsigned int _vbo_usage, int _coordCountByVertex, GLenum _drawUsage) : primitiveType(_primitiveType), coordCountByVertex(_coordCountByVertex), vbo_usage(_vbo_usage), vbo_index(0), vbo_vertices(0), vbo_uvs(0), vbo_normals(0), vbo_tangents(0), drawUsage(_drawUsage)
{
	subMeshCount = 1;
	totalTriangleCount = 0;
	triangleCount.push_back(0);
	indexOffsets.push_back(0);
}

Mesh::Mesh(const std::string& _path) : primitiveType(GL_TRIANGLES), coordCountByVertex(3), vbo_usage(USE_INDEX | USE_VERTICES | USE_UVS | USE_NORMALS | USE_TANGENTS), vbo_index(0), vbo_vertices(0), vbo_uvs(0), vbo_normals(0), vbo_tangents(0), drawUsage(GL_STATIC_DRAW)
{
	path = _path;

	subMeshCount = 1;
	totalTriangleCount = 0;
	triangleCount.push_back(0);
	indexOffsets.push_back(0);

	bool Ret = false;
	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (pScene) {
		Ret = initFromScene(pScene, path);
	}
	else {
		std::cout << "Error parsing " << path << " : " << Importer.GetErrorString() << std::endl;
	}
}

Mesh::~Mesh()
{
	freeGl();
}

//initialize vbos and vao, based on the informations of the mesh.
void Mesh::initGl()
{
	totalTriangleCount = triangleIndex.size() / 3;
	if(triangleCount.size() == 1) //only one mesh, we have to ensure it contains all triangles
		triangleCount[0] = (vbo_usage & USE_INDEX) ? triangleIndex.size() / 3 : vertices.size() / 9;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if (USE_INDEX & vbo_usage)
	{
		glGenBuffers(1, &vbo_index);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndex.size()*sizeof(int), &triangleIndex[0], GL_STATIC_DRAW);
	}

	if (USE_VERTICES & vbo_usage)
	{
		glGenBuffers(1, &vbo_vertices);
		glEnableVertexAttribArray(VERTICES);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], drawUsage);
		glVertexAttribPointer(VERTICES, coordCountByVertex, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * coordCountByVertex, (void*)0);
	}

	if (USE_NORMALS & vbo_usage)
	{
		glGenBuffers(1, &vbo_normals);
		glEnableVertexAttribArray(NORMALS);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), &normals[0], drawUsage);
		glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
	}

	if (USE_TANGENTS & vbo_usage)
	{
		glGenBuffers(1, &vbo_tangents);
		glEnableVertexAttribArray(TANGENTS);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
		glBufferData(GL_ARRAY_BUFFER, tangents.size()*sizeof(float), &tangents[0], drawUsage);
		glVertexAttribPointer(TANGENTS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
	}

	if (USE_UVS & vbo_usage)
	{
		glGenBuffers(1, &vbo_uvs);
		glEnableVertexAttribArray(UVS);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
		glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(float), &uvs[0], GL_STATIC_DRAW);
		glVertexAttribPointer(UVS, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);
	}
	/*
	if (USE_INSTANTIATION & vbo_usage)
	{
		glGenBuffers(4, vbo_transforms);
		glEnableVertexAttribArray(INSTANCE_TRANSFORM);
		glEnableVertexAttribArray(INSTANCE_TRANSFORM+1);
		glEnableVertexAttribArray(INSTANCE_TRANSFORM+2);
		glEnableVertexAttribArray(INSTANCE_TRANSFORM+3);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_transforms[0]);
		glBufferData(GL_ARRAY_BUFFER, transforms.size()*sizeof(float), &transforms[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_transforms[1]);
		glBufferData(GL_ARRAY_BUFFER, transforms.size()*sizeof(float), &transforms[1], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_transforms[2]);
		glBufferData(GL_ARRAY_BUFFER, transforms.size()*sizeof(float), &transforms[2], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_transforms[3]);
		glBufferData(GL_ARRAY_BUFFER, transforms.size()*sizeof(float), &transforms[3], GL_STATIC_DRAW);
		glVertexAttribPointer(INSTANCE_TRANSFORM, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
		glVertexAttribPointer(INSTANCE_TRANSFORM+1, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
		glVertexAttribPointer(INSTANCE_TRANSFORM+2, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
		glVertexAttribPointer(INSTANCE_TRANSFORM+3, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
	}*/

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::freeGl()
{
	if (vbo_index != 0)
		glDeleteBuffers(1, &vbo_index);

	if (vbo_vertices != 0)
		glDeleteBuffers(1, &vbo_vertices);

	if (vbo_uvs != 0)
		glDeleteBuffers(1, &vbo_uvs);

	if (vbo_normals != 0)
		glDeleteBuffers(1, &vbo_normals);

	if (vbo_tangents != 0)
		glDeleteBuffers(1, &vbo_tangents);

	glDeleteVertexArrays(1, &vao);
}

void Mesh::updateVBO(Vbo_types type)
{
	if (type == Vbo_types::INDEX && (USE_INDEX & vbo_usage))
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndex.size()*sizeof(int), &triangleIndex[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	if (type == Vbo_types::VERTICES && (USE_VERTICES & vbo_usage))
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], drawUsage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (type == Vbo_types::NORMALS && (USE_NORMALS & vbo_usage))
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), &normals[0], drawUsage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (type == Vbo_types::TANGENTS && (USE_TANGENTS & vbo_usage))
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
		glBufferData(GL_ARRAY_BUFFER, tangents.size()*sizeof(float), &tangents[0], drawUsage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (type == Vbo_types::UVS && (USE_UVS & vbo_usage))
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
		glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(float), &uvs[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Mesh::updateAllVBOs()
{
	if (USE_INDEX & vbo_usage)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndex.size()*sizeof(int), &triangleIndex[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	if (USE_VERTICES & vbo_usage)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], drawUsage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (USE_NORMALS & vbo_usage)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), &normals[0], drawUsage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (USE_TANGENTS & vbo_usage)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
		glBufferData(GL_ARRAY_BUFFER, tangents.size()*sizeof(float), &tangents[0], drawUsage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (USE_UVS & vbo_usage)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
		glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(float), &uvs[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

// simply draw the vertices, using vao.
void Mesh::draw()
{
	glBindVertexArray(vao);
	if (USE_INDEX & vbo_usage)
		glDrawElements(primitiveType, totalTriangleCount * 3, GL_UNSIGNED_INT, (GLvoid*)0);
	else
		glDrawArrays(primitiveType, 0, vertices.size() / 3);
	glBindVertexArray(0);
}

void Mesh::draw(int idx)
{
	glBindVertexArray(vao);
	if (USE_INDEX & vbo_usage)
		glDrawElements(primitiveType, triangleCount[idx]*3, GL_UNSIGNED_INT, (void*)(indexOffsets[idx]*sizeof(unsigned int)) );
	else
		glDrawArrays(primitiveType, indexOffsets[idx], triangleCount[idx] * 3);
	glBindVertexArray(0);
}

void Mesh::computeBoundingBox()
{
	//initialization : 
	//bottom left
		bottomLeft.x = vertices[0];
		bottomLeft.y = vertices[1];
		bottomLeft.z = vertices[2];
	//top right
		topRight.x = vertices[0];
		topRight.y = vertices[1];
		topRight.z = vertices[2];

	for (int i = 3; i < vertices.size(); i+=3)
	{
		//bottom left
		if (vertices[i] < bottomLeft.x)
			bottomLeft.x = vertices[i];
		if (vertices[i+1] < bottomLeft.y)
			bottomLeft.y = vertices[i+1];
		if (vertices[i+2] < bottomLeft.z)
			bottomLeft.z = vertices[i+2];

		//top right
		if (vertices[i] > topRight.x)
			topRight.x = vertices[i];
		if (vertices[i + 1] > topRight.y)
			topRight.y = vertices[i + 1];
		if (vertices[i + 2] > topRight.z)
			topRight.z = vertices[i + 2];
	}

	origin = bottomLeft + (topRight - bottomLeft) * 0.5f;
}

bool Mesh::isIntersectedByRay(const Ray & ray, CollisionInfo & collisionInfo) const
{
	bool isColliding = false;

	for (int i = 0; i < triangleIndex.size(); i+=3)
	{
		glm::vec3 a = vertexFrom3Floats(vertices, i);
		glm::vec3 b = vertexFrom3Floats(vertices, i+1);
		glm::vec3 c = vertexFrom3Floats(vertices, i+2);
		isColliding = ray.intersectTriangle(a, b, c, collisionInfo);
		if (isColliding)
			return true;
	}
	return false;
}


bool Mesh::initFromScene(const aiScene* pScene, const std::string& Filename)
{
	triangleIndex.clear();
	uvs.clear();
	vertices.clear();
	normals.clear();
	tangents.clear();

	totalTriangleCount = 0;
	triangleCount.clear();
	indexOffsets.clear();
	subMeshCount = pScene->mNumMeshes;
	
	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i <subMeshCount; i++)
	{
		indexOffsets.push_back(totalTriangleCount*3);
		const aiMesh* paiMesh = pScene->mMeshes[i];
		initMesh(i, paiMesh);
		triangleCount.push_back((triangleIndex.size() / 3) - totalTriangleCount);
		totalTriangleCount += triangleCount.back();

	}
	initGl(); //don't forget to init mesh for opengl
	computeBoundingBox();

	return true;
}

void Mesh::initMesh(unsigned int Index, const aiMesh* paiMesh)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	//bounds init :
	if (paiMesh->mNumVertices > 0)
	{
		const aiVector3D* pPos00 = &(paiMesh->mVertices[0]);
		topRight = glm::vec3(pPos00->x, pPos00->y, pPos00->z);
		bottomLeft = glm::vec3(pPos00->x, pPos00->y, pPos00->z);
	}

	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) 
	{
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : &Zero3D;
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		const aiVector3D* pTangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mTangents[i]) : &Zero3D;

		vertices.push_back(pPos->x);
		vertices.push_back(pPos->y);
		vertices.push_back(pPos->z);

		normals.push_back(pNormal->x);
		normals.push_back(pNormal->y);
		normals.push_back(pNormal->z);

		uvs.push_back(pTexCoord->x);
		uvs.push_back(pTexCoord->y);

		tangents.push_back(pTangent->x);
		tangents.push_back(pTangent->y);
		tangents.push_back(pTangent->z);

		//bounds :
		/*if (pPos->x > topRight.x)
			topRight.x = pPos->x;
		if (pPos->y > topRight.y)
			topRight.y = pPos->y;
		if (pPos->z > topRight.z)
			topRight.z = pPos->z;

		if (pPos->x < bottomLeft.x)
			bottomLeft.x = pPos->x;
		if (pPos->y < bottomLeft.y)
			bottomLeft.y = pPos->y;
		if (pPos->z < bottomLeft.z)
			bottomLeft.z = pPos->z;

		origin = -glm::normalize(topRight - bottomLeft) * 0.5f;*/
	}

	int offsetGlIdx = triangleIndex.size();
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) 
	{
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		triangleIndex.push_back(offsetGlIdx + Face.mIndices[0]);
		triangleIndex.push_back(offsetGlIdx + Face.mIndices[1]);
		triangleIndex.push_back(offsetGlIdx + Face.mIndices[2]);
	}

}
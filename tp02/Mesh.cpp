#include "Mesh.h"

Mesh::Mesh(GLenum _primitiveType , unsigned int _vbo_usage, int _coordCountByVertex) : primitiveType(_primitiveType), coordCountByVertex(_coordCountByVertex), vbo_usage(_vbo_usage), triangleCount(0), vbo_index(0), vbo_vertices(0), vbo_uvs(0), vbo_normals(0), vbo_tangents(0)
{

}

Mesh::Mesh(const std::string& path) : primitiveType(GL_TRIANGLES), coordCountByVertex(3), vbo_usage(USE_INDEX | USE_VERTICES | USE_UVS | USE_NORMALS | USE_TANGENTS), triangleCount(0), vbo_index(0), vbo_vertices(0), vbo_uvs(0), vbo_normals(0), vbo_tangents(0)
{
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

//initialize vbos and vao, based on the informations of the mesh.
void Mesh::initGl()
{
	triangleCount = triangleIndex.size() / 3;

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
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(VERTICES, coordCountByVertex, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * coordCountByVertex, (void*)0);
	}

	if (USE_NORMALS & vbo_usage)
	{
		glGenBuffers(1, &vbo_normals);
		glEnableVertexAttribArray(NORMALS);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
	}

	if (USE_TANGENTS & vbo_usage)
	{
		glGenBuffers(1, &vbo_tangents);
		glEnableVertexAttribArray(TANGENTS);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
		glBufferData(GL_ARRAY_BUFFER, tangents.size()*sizeof(float), &tangents[0], GL_STATIC_DRAW);
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

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// simply draw the vertices, using vao.
void Mesh::draw()
{
	glBindVertexArray(vao);
	if (USE_INDEX & vbo_usage)
		glDrawElements(primitiveType, triangleCount * 3, GL_UNSIGNED_INT, (GLvoid*)0);
	else
		glDrawArrays(primitiveType, 0, vertices.size());
	glBindVertexArray(0);
}


bool Mesh::initFromScene(const aiScene* pScene, const std::string& Filename)
{
	int numMesh = pScene->mNumMeshes;

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i <numMesh; i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		initMesh(i, paiMesh);
	}

	return true;
}

void Mesh::initMesh(unsigned int Index, const aiMesh* paiMesh)
{
	triangleIndex.clear();
	uvs.clear();
	vertices.clear();
	normals.clear();
	tangents.clear();

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	//bounds init :
	if (paiMesh->mNumVertices > 0)
	{
		const aiVector3D* pPos00 = &(paiMesh->mVertices[0]);
		topRight = glm::vec3(pPos00->x, pPos00->y, pPos00->z);
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
		if (pPos->x > topRight.x)
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
	}

	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) 
	{
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		triangleIndex.push_back(Face.mIndices[0]);
		triangleIndex.push_back(Face.mIndices[1]);
		triangleIndex.push_back(Face.mIndices[2]);
	}

	initGl();
}
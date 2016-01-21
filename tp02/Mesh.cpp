#include "Mesh.h"

Mesh::Mesh(GLenum _primitiveType , unsigned int _vbo_usage, int _coordCountByVertex) : primitiveType(_primitiveType), coordCountByVertex(_coordCountByVertex), vbo_usage(_vbo_usage), triangleCount(0), vbo_index(0), vbo_vertices(0), vbo_uvs(0), vbo_normals(0)
{

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
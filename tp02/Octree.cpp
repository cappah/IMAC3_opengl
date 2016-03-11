#include "Octree.h"

OctreeDrawer::OctreeDrawer()
{
}

void OctreeDrawer::initGl()
{
	triangleCount = triangleIndex.size() / 3;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	glGenBuffers(1, &vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndex.size()*sizeof(int), &triangleIndex[0], GL_STATIC_DRAW);


	glGenBuffers(1, &vbo_vertices);
	glEnableVertexAttribArray(VERTICES);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(VERTICES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);


	glGenBuffers(1, &vbo_normals);
	glEnableVertexAttribArray(NORMALS);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);


	glGenBuffers(1, &vbo_uvs);
	glEnableVertexAttribArray(UVS);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
	glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(float), &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(UVS, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);


	glGenBuffers(1, &vbo_pos);
	glEnableVertexAttribArray(POSITIONS);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
	glVertexAttribPointer(POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

	//for physic simulation : 
	glGenBuffers(1, &vbo_animPos);
	glEnableVertexAttribArray(ANIM_POS);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_animPos);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(ANIM_POS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OctreeDrawer::render(const glm::mat4 & projection, const glm::mat4 & view)
{
	glm::mat4 VP = projection * view;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTexture->glId);

	materialGrassField.use();
	materialGrassField.setUniformTime(0); //TODO : ADD TIME
	materialGrassField.setUniformTexture(0);
	materialGrassField.setUniformVP(VP);

	draw();
}

void OctreeDrawer::draw()
{
	if (positions.size() == 0)
		return;

	int instanceCount = positions.size() / 3.f;

	glBindVertexArray(vao);

	glVertexAttribDivisor(VERTICES, 0);
	glVertexAttribDivisor(NORMALS, 0);
	glVertexAttribDivisor(UVS, 0);
	glVertexAttribDivisor(POSITIONS, 1);
	glVertexAttribDivisor(ANIM_POS, 1);

	glDrawElementsInstanced(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_INT, (GLvoid*)0, instanceCount);

	glBindVertexArray(0);
}

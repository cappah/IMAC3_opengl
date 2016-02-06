#include "Terrain.h"
#include "Factories.h" //forward

Terrain::Terrain(float width, float height, float depth, int subdivision, glm::vec3 offset) : m_terrainFbo(0), m_terrainMaterial(ProgramFactory::get().get("defaultTerrain")), m_quadMesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES), 2) , m_noiseTexture(1024, 1024, glm::vec4(0.f,0.f,0.f,1.f)), m_terrainTexture(1024, 1024, glm::vec4(1, 1, 1, 1)), m_filterTexture(1024, 1024, glm::vec4(0, 0, 0, 1)), m_width(width), m_height(height), m_depth(depth), m_subdivision(subdivision), m_offset(offset), m_seed(0)
{
	m_terrainTexture.initGL();
	m_noiseTexture.initGL();

	//set the terrain texture as diffuse texture : 
	m_material.setDiffuse(&m_terrainTexture);

	////////////////////// INIT QUAD MESH ////////////////////////
	m_quadMesh.triangleIndex = { 0, 1, 2, 2, 1, 3 };
	m_quadMesh.vertices = { -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0 };
	m_quadMesh.initGl();

	m_terrainNoise.seed = m_seed;

	generateTerrain();
	applyNoise(m_terrainNoise.generatePerlin2D());


	//generate terrain FBO : 
	glGenFramebuffers(1, &m_terrainFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_terrainFbo);

	// Attach textures to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_terrainTexture.glId, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "Error on building framebuffer\n");
		exit(EXIT_FAILURE);
	}

	// Back to the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


Terrain::~Terrain()
{
	m_material.setDiffuse(nullptr); // detach texture as the texture is inside the terrain and will be destroyed

	if (vbo_index != 0)
		glDeleteBuffers(1, &vbo_index);

	if (vbo_vertices != 0)
		glDeleteBuffers(1, &vbo_vertices);

	if (vbo_uvs != 0)
		glDeleteBuffers(1, &vbo_uvs);

	if (vbo_normals != 0)
		glDeleteBuffers(1, &vbo_normals);

	glDeleteVertexArrays(1, &vao);

	glDeleteFramebuffers(1, &m_terrainFbo);

	m_terrainTexture.freeGL(); 
	m_noiseTexture.freeGL();
}


void Terrain::generateTerrainTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_terrainFbo);

	glClear(GL_COLOR_BUFFER_BIT);

	m_terrainMaterial.use(); // textureRepetition, TODO

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_filterTexture.glId);

	for (int i = 0; i < m_terrainLayouts.size(); i++)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_filterTexture.glId);

		m_terrainMaterial.setUniformFilterTexture(0);
		m_terrainMaterial.setUniformDiffuseTexture(1);
		m_terrainMaterial.setUniformLayoutOffset( glm::vec2( 255.f*i / (float)m_terrainLayouts.size(), 255.f*(i+1) / (float)m_terrainLayouts.size() ) );

		m_quadMesh.draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Terrain::computeNoiseTexture(Perlin2D& perlin2D)
{
	//redraw the noise texture
	m_noiseTexture.freeGL();

	for (int j = 0, k = 0; j < 1024; j++)
	{
		for (int i = 0; i < 1024; i++, k++)
		{
			float x = (i * m_subdivision) / 1024.f;
			float y = (j * m_subdivision) / 1024.f;

			m_noiseTexture.pixels[k] = perlin2D.getNoiseValue(x, y) * 2.f - 1.f;
		}
	}

	m_noiseTexture.initGL();
}

void Terrain::computeNormals()
{
	glm::vec3 u(1, 0, 0);
	glm::vec3 v(0, 0, 1);
	glm::vec3 normal(0, 0, 0);
	glm::vec3 tangent(0, 0, 0);

	for (int j = 0, k = 0; j < m_subdivision; j++)
	{
		for (int i = 0; i < m_subdivision; i++, k+=3)
		{
			if (i > 0 && i < m_subdivision - 1 && j > 0 && j < m_subdivision - 1)
			{
				if (j - 1 >= 0 && i - 1 >= 0)
				{
					u = vertexFrom3Floats(m_vertices, i + (j - 1) * m_subdivision) - vertexFrom3Floats(m_vertices, i + j * m_subdivision);
					v = vertexFrom3Floats(m_vertices, (i - 1)+ j * m_subdivision) - vertexFrom3Floats(m_vertices, i + j * m_subdivision);
					normal += glm::normalize(glm::cross(u, v));
				}

				if (i - 1 >= 0 && j + 1 < (m_subdivision ))
				{
					u = vertexFrom3Floats(m_vertices, (i - 1) + j * m_subdivision) - vertexFrom3Floats(m_vertices, i + j * m_subdivision);
					v = vertexFrom3Floats(m_vertices, i + (j + 1) * m_subdivision) - vertexFrom3Floats(m_vertices, i + j * m_subdivision);
					normal += glm::normalize(glm::cross(u, v));
				}

				if (j + 1 < (m_subdivision ) && i+1 < (m_subdivision ))
				{
					u = vertexFrom3Floats(m_vertices, i + (j + 1) * m_subdivision) - vertexFrom3Floats(m_vertices, i + j * m_subdivision);
					v = vertexFrom3Floats(m_vertices, (i + 1) + j * m_subdivision) - vertexFrom3Floats(m_vertices, i + j * m_subdivision);
					normal += glm::normalize(glm::cross(u, v));
				}

				if (i + 1 < (m_subdivision ) && j - 1 >= 0)
				{
					u = vertexFrom3Floats(m_vertices, (i + 1) + j * m_subdivision) - vertexFrom3Floats(m_vertices, i + j * m_subdivision);
					v = vertexFrom3Floats(m_vertices, i + (j - 1) * m_subdivision) - vertexFrom3Floats(m_vertices, i + j * m_subdivision);

					normal += glm::normalize(glm::cross(u, v));
				}

				normal = glm::normalize(normal);
			}
			else
				normal = glm::vec3(0, 1, 0);


			m_normals[k] = normal.x;
			m_normals[k+1] = normal.y;
			m_normals[k+2] = normal.z;

			tangent = glm::normalize(glm::cross(normal, u));

			m_tangents[k] = tangent.x;
			m_tangents[k+1] = tangent.y;
			m_tangents[k+2] = tangent.z;
		}
	}


	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(float), &m_normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
	glBufferData(GL_ARRAY_BUFFER, m_tangents.size()*sizeof(float), &m_tangents[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::applyNoise(Perlin2D& perlin2D)
{
	for (int j = 0, k = 1, l = 0; j < m_subdivision; j++)
	{
		for (int i = 0; i < m_subdivision; i++, k += 3, l++)
		{
			m_heightMap[l] = perlin2D.getNoiseValue(i, j) * 2.f - 1.f;

			m_vertices[k] = m_heightMap[l] * m_height + m_offset.y;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(float), &m_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	computeNormals();

	//refresh the terrain texture : 
	computeNoiseTexture(perlin2D);
	generateTerrainTexture();
}

void Terrain::generateTerrain()
{
	float paddingZ = m_depth / (float)m_subdivision;
	float paddingX = m_width / (float)m_subdivision;

	int lineCount = (m_subdivision - 1);
	int rowCount = (m_subdivision - 1);
	m_triangleCount = (m_subdivision - 1) * (m_subdivision - 1) * 2 + 1;

	m_vertices.clear();
	m_normals.clear();
	m_uvs.clear();
	m_triangleIndex.clear();
	m_heightMap.clear();
	m_tangents.clear();

	for (int j = 0; j < m_subdivision; j++)
	{
		for (int i = 0; i < m_subdivision; i++)
		{
			m_heightMap.push_back(0);

			m_vertices.push_back(i*paddingX + m_offset.x);
			m_vertices.push_back(m_offset.y);
			m_vertices.push_back(j*paddingZ + m_offset.z);

			m_normals.push_back(0);
			m_normals.push_back(1);
			m_normals.push_back(0);

			m_tangents.push_back(1);
			m_tangents.push_back(0);
			m_tangents.push_back(0);

			m_uvs.push_back(i / (float)(m_subdivision - 1));
			m_uvs.push_back(j / (float)(m_subdivision - 1));
		}
	}

	for (int i = 0, k = 0; i < m_triangleCount; i++)
	{


		if (i % 2 == 0)
		{
			m_triangleIndex.push_back(k + 0);
			m_triangleIndex.push_back(k + 1);
			m_triangleIndex.push_back(k + m_subdivision);
		}
		else
		{
			m_triangleIndex.push_back(k + 1);
			m_triangleIndex.push_back(k + m_subdivision + 1);
			m_triangleIndex.push_back(k + m_subdivision);
		}

		if (i % 2 == 0 && i != 0)
			k++;

		if ((k + 1) % (m_subdivision) == 0 && i != 0)
		{
			k++;
		}
	}

	initGl();
}

void Terrain::updateTerrain()
{
	float paddingZ = m_depth / (float)m_subdivision;
	float paddingX = m_width / (float)m_subdivision;

	for (int j = 0, k = 0, l = 0; j < m_subdivision; j++)
	{
		for (int i = 0; i < m_subdivision; i++, k += 3, l++)
		{
			m_vertices[k] = i*paddingX + m_offset.x;
			m_vertices[k+1] = m_heightMap[l] * m_height + m_offset.y;
			m_vertices[k+2] = j*paddingZ + m_offset.z;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(float), &m_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//initialize vbos and vao, based on the informations of the mesh.
void Terrain::initGl()
{

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	glGenBuffers(1, &vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangleIndex.size()*sizeof(int), &m_triangleIndex[0], GL_STATIC_DRAW);


	glGenBuffers(1, &vbo_vertices);
	glEnableVertexAttribArray(VERTICES);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(float), &m_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(VERTICES, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);



	glGenBuffers(1, &vbo_normals);
	glEnableVertexAttribArray(NORMALS);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(float), &m_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);

	glGenBuffers(1, &vbo_tangents);
	glEnableVertexAttribArray(TANGENTS);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
	glBufferData(GL_ARRAY_BUFFER, m_tangents.size()*sizeof(float), &m_tangents[0], GL_STATIC_DRAW);
	glVertexAttribPointer(TANGENTS, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);


	glGenBuffers(1, &vbo_uvs);
	glEnableVertexAttribArray(UVS);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
	glBufferData(GL_ARRAY_BUFFER, m_uvs.size()*sizeof(float), &m_uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(UVS, 2, GL_FLOAT , GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);


	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// simply draw the vertices, using vao.
void Terrain::render(const glm::mat4& projection, const glm::mat4& view)
{
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1), m_offset);
	glm::mat4 mvp = projection * view * modelMatrix;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

	m_material.setUniform_MVP(mvp);
	m_material.setUniform_normalMatrix(normalMatrix);

	m_material.use();

	glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, m_triangleCount * 3, GL_UNSIGNED_INT, (GLvoid*)0);
	glBindVertexArray(0);
}

void Terrain::drawUI()
{

	if (ImGui::InputInt("terrain seed", &m_seed))
	{
		// change the seed of the generator
		m_terrainNoise.seed = m_seed;
		//apply new noise to terrain
		applyNoise(m_terrainNoise.generatePerlin2D());
	}

	if (ImGui::InputFloat3("terrain offset", &m_offset[0]))
	{
		updateTerrain();
	}

	glm::vec3 terrainDim(m_width, m_height, m_depth);
	if (ImGui::InputFloat3("terrain dimensions", &terrainDim[0]))
	{
		m_width = terrainDim.x;
		m_height = terrainDim.y;
		m_depth = terrainDim.z;

		updateTerrain();
	}

	if (ImGui::InputInt("terrain subdivision", &m_subdivision))
	{
		generateTerrain();
		applyNoise(m_terrainNoise.generatePerlin2D());
	}

	//ImGui::PushID("terrainMaterial");
	//m_material.drawUI();
	//ImGui::PopID();
	
	if (ImGui::SliderFloat("noise persistence", &m_terrainNoise.persistence, 0.f, 1.f))
	{
		applyNoise(m_terrainNoise.generatePerlin2D());
	}
	if (ImGui::SliderInt("noise octave count", &m_terrainNoise.octaveCount, 1, 10))
	{
		applyNoise(m_terrainNoise.generatePerlin2D());
	}
	if (ImGui::SliderInt("noise height", &m_terrainNoise.height, 1, 512))
	{
		applyNoise(m_terrainNoise.generatePerlin2D());
	}
	if (ImGui::SliderInt("noise sampling offset", &m_terrainNoise.samplingOffset, 1, 128))
	{
		applyNoise(m_terrainNoise.generatePerlin2D());
	}


	ImGui::InputText("new texture layout", m_newTextureName, 30);
	ImGui::SameLine();
	if (ImGui::SmallButton("add"))
	{
		if (TextureFactory::get().contains(m_newTextureName))
		{
			m_terrainLayouts.push_back(TextureFactory::get().get(m_newTextureName));
			m_terrainLayouts.back()->initGL();
		}
	}

	for (int i = 0; i < m_terrainLayouts.size(); i++)
	{
		ImGui::Text(m_terrainLayouts[i]->name.c_str());
		ImGui::SameLine();
		if (ImGui::SmallButton("remove"))
		{
			m_terrainLayouts[i]->freeGL();
			m_terrainLayouts.erase(m_terrainLayouts.begin() + i);

		}
	}
}
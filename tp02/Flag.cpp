#include "Flag.h"

namespace Physic {

	Flag::Flag(Material* material, int subdivision, float width, float height) : m_material(material), m_subdivision(subdivision), m_width(width), m_height(height)
	{

		float paddingX = m_width / (float)m_subdivision;
		float paddingY = m_height / (float)m_subdivision;

		int lineCount = (m_subdivision - 1);
		int rowCount = (m_subdivision - 1);
		m_mesh.triangleCount = (m_subdivision - 1) * (m_subdivision - 1) * 2 + 1;

		m_mesh.vertices.clear();
		m_mesh.normals.clear();
		m_mesh.uvs.clear();
		m_mesh.triangleIndex.clear();
		m_mesh.tangents.clear();

		pointContainer.clear();
		linkShape.clear();
		linkShearing.clear();
		linkBlending.clear();

		for (int j = 0; j < m_subdivision; j++)
		{
			for (int i = 0; i < m_subdivision; i++)
			{
				//physic elements : 
				pointContainer.push_back( Point(glm::vec3(i*paddingX, 0, j*paddingY), glm::vec3(0,0,0), 0.f));

				//visual elements : 
				m_mesh.vertices.push_back(i*paddingX);
				m_mesh.vertices.push_back(0);
				m_mesh.vertices.push_back(j*paddingY);

				m_mesh.normals.push_back(0);
				m_mesh.normals.push_back(1);
				m_mesh.normals.push_back(0);

				m_mesh.tangents.push_back(0);
				m_mesh.tangents.push_back(0);
				m_mesh.tangents.push_back(1);

				m_mesh.uvs.push_back(i / (float)(m_subdivision - 1));
				m_mesh.uvs.push_back(j / (float)(m_subdivision - 1));
			}
		}

		for (int i = 0, k = 0; i < m_mesh.triangleCount; i++)
		{
			if (i % 2 == 0)
			{
				m_mesh.triangleIndex.push_back(k + 0);
				m_mesh.triangleIndex.push_back(k + 1);
				m_mesh.triangleIndex.push_back(k + m_subdivision);
			}
			else
			{
				m_mesh.triangleIndex.push_back(k + 1);
				m_mesh.triangleIndex.push_back(k + m_subdivision + 1);
				m_mesh.triangleIndex.push_back(k + m_subdivision);
			}

			if (i % 2 == 0 && i != 0)
				k++;

			if ((k + 1) % (m_subdivision) == 0 && i != 0)
			{
				k++;
			}
		}


		m_mesh.initGl();

		//intialyze physic links : 
		for (int j = 1; j < m_subdivision - 1; j++)
		{
			for (int i = 1; i < m_subdivision - 1; i++)
			{
				Point* current = &pointContainer[ idx2DToIdx1D(i, j, m_subdivision) ];
				Point* left = &pointContainer[ idx2DToIdx1D(i-1, j, m_subdivision) ];
				Point* right = &pointContainer[ idx2DToIdx1D(i+1, j, m_subdivision) ];
				Point* up = &pointContainer[ idx2DToIdx1D(i, j+1, m_subdivision) ];
				Point* down = &pointContainer[ idx2DToIdx1D(i, j-1, m_subdivision) ];

				Point* leftDown = &pointContainer[idx2DToIdx1D(i-1, j-1, m_subdivision)];
				Point* leftUp = &pointContainer[idx2DToIdx1D(i-1, j+1, m_subdivision)];
				Point* rightDown = &pointContainer[idx2DToIdx1D(i+1, j-1, m_subdivision)];
				Point* rightUp = &pointContainer[idx2DToIdx1D(i+1, j+1, m_subdivision)];


				// shape links : 
				linkShape.push_back(Link(left, current)); //left link
				linkShape.push_back(Link(current, right)); //right link
				linkShape.push_back(Link(down, current)); //down link
				linkShape.push_back(Link(current, up)); //up link

				// shearing links : 
				linkShearing.push_back(Link(leftDown, current)); //left down link
				linkShearing.push_back(Link(leftUp, current)); //left up link
				linkShearing.push_back(Link(current, rightDown)); //right down link
				linkShearing.push_back(Link(current, rightUp)); //right up link

				//blending links : 
				if (i - 2 >= 0)
				{
					Point* left2 = &pointContainer[idx2DToIdx1D(i - 2, j, m_subdivision)];
					linkShearing.push_back(Link(left2, current)); //left link
				}
				if (i + 2 < m_subdivision)
				{
					Point* right2 = &pointContainer[idx2DToIdx1D(i + 2, j, m_subdivision)];
					linkShearing.push_back(Link(current, right2)); //right link
				}
				if (j - 2 >= 0)
				{
					Point* down2 = &pointContainer[idx2DToIdx1D(i, j - 2, m_subdivision)];
					linkShearing.push_back(Link(down2, current)); //down link
				}
				if (j + 2 < m_subdivision)
				{
					Point* up2 = &pointContainer[idx2DToIdx1D(i, j + 2, m_subdivision)];
					linkShearing.push_back(Link(current, up2)); //up link
				}
			}
		}

	}

	void Physic::Flag::update(float deltaTime)
	{
		//points : 
		for (int i = 0; i < pointContainer.size(); i++)
			computePoints(deltaTime, &pointContainer[i]);

		//shape : 
		for (int i = 0; i < linkShape.size(); i++)
			computeLinks(deltaTime, &linkShape[i]);

		//shearing :
		for (int i = 0; i < linkShearing.size(); i++)
			computeLinks(deltaTime, &linkShearing[i]);

		//blending : 
		for (int i = 0; i < linkBlending.size(); i++)
			computeLinks(deltaTime, &linkBlending[i]);

		synchronizeVisual();
	}

	void Flag::synchronizeVisual()
	{
		//TODO
	}

	void Flag::computeLinks(float deltaTime, Link* link)
	{
		float d = glm::distance(link->M1->position, link->M2->position);
		if (d < 0.00000001f)
			return;

		float f = link->k * (1.f - link->l / d);
		if (std::abs(f) < 0.00000001f)
			return;

		glm::vec3 M1M2 = link->M2->position - link->M1->position;
		glm::normalize(M1M2);
		if (glm::length(M1M2) < 0.00000001f)
			return;

		//frein :
		glm::vec3 frein = link->z*(link->M2->vitesse - link->M1->vitesse);

		link->M1->force += (f * M1M2 + frein);
		link->M2->force += (-f * M1M2 - frein);

		std::cout << "force de rappel en M1 : " << link->M1->force.x << ", " << link->M1->force.y << std::endl;
		std::cout << "force de rappel en M2 : " << link->M2->force.x << ", " << link->M2->force.y << std::endl;
	}

	void Flag::computePoints(float deltaTime, Point* point)
	{
		if (point->masse < 0.000001f)
			return;

		//leapfrog
		point->vitesse += (deltaTime / point->masse)*point->force;
		point->position += deltaTime*point->vitesse;
		point->force = glm::vec3(0, 0, 0);
	}

	glm::vec3 Physic::Flag::getGravity() const
	{
		return glm::vec3();
	}

	void Physic::Flag::setGravity(const glm::vec3 & _gravity)
	{
	}

	void Physic::Flag::render(const glm::mat4& projection, const glm::mat4& view)
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		glm::mat4 mvp = projection * view * modelMatrix;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

		m_material.setUniform_MVP(mvp);
		m_material.setUniform_normalMatrix(normalMatrix);

		m_material.use();

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, m_triangleCount * 3, GL_UNSIGNED_INT, (GLvoid*)0);
		glBindVertexArray(0);
	}


	Flag::~Flag()
	{
	}

}

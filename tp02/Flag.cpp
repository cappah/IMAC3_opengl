#include "Flag.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

namespace Physic {

	Flag::Flag(Material3DObject* material, int subdivision, float width, float height) : Component(FLAG), m_material(material), m_subdivision(subdivision), m_width(width), m_height(height)
	{
		modelMatrix = glm::mat4(1);

		//don't forget to change the origin to have the right pivot rotation
		origin = glm::vec3(-0.5f, -0.5f, 0.f);
		m_mesh.origin = glm::vec3(-0.5f, -0.5f, 0.f);

		m_mesh.topRight = glm::vec3(m_width, m_height, 0.f);
		m_mesh.bottomLeft = glm::vec3(0.f, 0.f, 0.f);

		m_mesh.vertices.clear();
		m_mesh.normals.clear();
		m_mesh.uvs.clear();
		m_mesh.triangleIndex.clear();
		m_mesh.tangents.clear();

		localPointPositions.clear();
		pointContainer.clear();
		linkShape.clear();
		linkShearing.clear();
		linkBlending.clear();

		generateMesh();

		m_mesh.initGl();

		initialyzePhysic();

		//cover the mesh with collider : 
		if (m_entity != nullptr)
		{
			auto collider = static_cast<Collider*>(m_entity->getComponent(Component::COLLIDER));
			if (collider != nullptr)
			{
				collider->coverMesh(m_mesh);
				collider->setOffsetScale(glm::vec3(1.f, 1.f, 2.f));
			}
		}

	}

	Flag::~Flag()
	{
	}

	void Flag::generateMesh()
	{
		float paddingX = m_width / (float)(m_subdivision - 1);
		float paddingY = m_height / (float)(m_subdivision - 1);

		for (int j = 0; j < m_subdivision; j++)
		{
			for (int i = 0; i < m_subdivision; i++)
			{
				//physic elements : 
				localPointPositions.push_back(glm::vec3(i*paddingX, j*paddingY, 0.f));
				pointContainer.push_back(Point(glm::vec3(i*paddingX, j*paddingY, 0.f), glm::vec3(0, 0, 0), 0.f));
			}
		}

		int lineCount = (m_subdivision - 1);
		int rowCount = (m_subdivision - 1);
		m_mesh.triangleCount = (m_subdivision - 1) * (m_subdivision - 1) * 2 + 1;

		// face 1 : 
		for (int j = 0; j < m_subdivision; j++)
		{
			for (int i = 0; i < m_subdivision; i++)
			{

				//visual elements : 
				m_mesh.vertices.push_back(i*paddingX);
				m_mesh.vertices.push_back(j*paddingY);
				m_mesh.vertices.push_back(0.1f);

				m_mesh.normals.push_back(0);
				m_mesh.normals.push_back(0);
				m_mesh.normals.push_back(1);

				m_mesh.tangents.push_back(1);
				m_mesh.tangents.push_back(0);
				m_mesh.tangents.push_back(0);

				m_mesh.uvs.push_back(i / (float)(m_subdivision - 1));
				m_mesh.uvs.push_back(j / (float)(m_subdivision - 1));
			}
		}

		// face 2 : 
		for (int j = 0; j < m_subdivision; j++)
		{
			for (int i = 0; i < m_subdivision; i++)
			{

				//visual elements : 
				m_mesh.vertices.push_back(i*paddingX);
				m_mesh.vertices.push_back(j*paddingY);
				m_mesh.vertices.push_back(-0.1f);

				m_mesh.normals.push_back(0);
				m_mesh.normals.push_back(0);
				m_mesh.normals.push_back(-1);

				m_mesh.tangents.push_back(1);
				m_mesh.tangents.push_back(0);
				m_mesh.tangents.push_back(0);

				m_mesh.uvs.push_back(i / (float)(m_subdivision - 1));
				m_mesh.uvs.push_back(j / (float)(m_subdivision - 1));
			}
		}

		// face 1 : 
		int k = 0;
		for (int i = 0; i < m_mesh.triangleCount; i++)
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

		k += m_subdivision;

		// face 2 : 
		for (int i = 0; i < m_mesh.triangleCount; i++)
		{
			if (i % 2 == 0)
			{
				m_mesh.triangleIndex.push_back(k + 0);
				m_mesh.triangleIndex.push_back(k + m_subdivision);
				m_mesh.triangleIndex.push_back(k + 1);
			}
			else
			{
				m_mesh.triangleIndex.push_back(k + 1);
				m_mesh.triangleIndex.push_back(k + m_subdivision);
				m_mesh.triangleIndex.push_back(k + m_subdivision + 1);
			}

			if (i % 2 == 0 && i != 0)
				k++;

			if ((k + 1) % (m_subdivision) == 0 && i != 0)
			{
				k++;
			}
		}

		m_mesh.triangleCount *= 2;

	}

	void Flag::updateNormals()
	{
		int verticesPerFace = m_subdivision * m_subdivision;

		glm::vec3 u(1, 0, 0);
		glm::vec3 v(0, 0, 1);
		glm::vec3 normal(0, 0, 0);
		glm::vec3 tangent(0, 0, 0);

		for (int j = 0, k = 0; j < m_subdivision; j++)
		{
			for (int i = 0; i < m_subdivision; i++, k += 3)
			{

				if (j - 1 >= 0 && i - 1 >= 0)
				{
					u = vertexFrom3Floats(m_mesh.vertices, i + (j - 1) * m_subdivision) - vertexFrom3Floats(m_mesh.vertices, i + j * m_subdivision);
					v = vertexFrom3Floats(m_mesh.vertices, (i - 1) + j * m_subdivision) - vertexFrom3Floats(m_mesh.vertices, i + j * m_subdivision);
					normal += glm::normalize(glm::cross(u, v));
				}

				if (i - 1 >= 0 && j + 1 < (m_subdivision))
				{
					u = vertexFrom3Floats(m_mesh.vertices, (i - 1) + j * m_subdivision) - vertexFrom3Floats(m_mesh.vertices, i + j * m_subdivision);
					v = vertexFrom3Floats(m_mesh.vertices, i + (j + 1) * m_subdivision) - vertexFrom3Floats(m_mesh.vertices, i + j * m_subdivision);
					normal += glm::normalize(glm::cross(u, v));
				}

				if (j + 1 < (m_subdivision) && i + 1 < (m_subdivision))
				{
					u = vertexFrom3Floats(m_mesh.vertices, i + (j + 1) * m_subdivision) - vertexFrom3Floats(m_mesh.vertices, i + j * m_subdivision);
					v = vertexFrom3Floats(m_mesh.vertices, (i + 1) + j * m_subdivision) - vertexFrom3Floats(m_mesh.vertices, i + j * m_subdivision);
					normal += glm::normalize(glm::cross(u, v));
				}

				if (i + 1 < (m_subdivision) && j - 1 >= 0)
				{
					u = vertexFrom3Floats(m_mesh.vertices, (i + 1) + j * m_subdivision) - vertexFrom3Floats(m_mesh.vertices, i + j * m_subdivision);
					v = vertexFrom3Floats(m_mesh.vertices, i + (j - 1) * m_subdivision) - vertexFrom3Floats(m_mesh.vertices, i + j * m_subdivision);

					normal += glm::normalize(glm::cross(u, v));
				}

				normal = glm::normalize(normal);


				//face 1 
				m_mesh.normals[k] = normal.x;
				m_mesh.normals[k + 1] = normal.y;
				m_mesh.normals[k + 2] = normal.z;

				//face 2 
				m_mesh.normals[k + verticesPerFace * 3] = -normal.x;
				m_mesh.normals[k + 1 + verticesPerFace * 3] = -normal.y;
				m_mesh.normals[k + 2 + verticesPerFace * 3] = -normal.z;

				tangent = glm::normalize(glm::cross(normal, u));

				//face 1 
				m_mesh.tangents[k] = tangent.x;
				m_mesh.tangents[k + 1] = tangent.y;
				m_mesh.tangents[k + 2] = tangent.z;

				//face 2 
				m_mesh.tangents[k + verticesPerFace * 3] = -tangent.x;
				m_mesh.tangents[k + 1 + verticesPerFace * 3] = -tangent.y;
				m_mesh.tangents[k + 2 + verticesPerFace * 3] = -tangent.z;
			}
		}


		m_mesh.updateVBO(Mesh::Vbo_types::NORMALS);
		m_mesh.updateVBO(Mesh::Vbo_types::TANGENTS);
	}

	void Flag::initialyzePhysic()
	{
		//intialyze physic links : 
		float k = 0.00002f;
		float z = 0.000002f;
		float l = 0.f;
		for (int j = 0; j < m_subdivision; j++)
		{
			for (int i = 0; i < m_subdivision; i++)
			{
				Point* current = &pointContainer[idx2DToIdx1D(i, j, m_subdivision)];

				//shape links

				if (i + 1 < m_subdivision)
				{
					Point* right = &pointContainer[idx2DToIdx1D(i + 1, j, m_subdivision)];
					l = glm::distance(right->position, current->position);
					linkShape.push_back(Link(current, right, k, z, l)); //right link
				}

				if (j + 1 < m_subdivision)
				{
					Point* up = &pointContainer[idx2DToIdx1D(i, j + 1, m_subdivision)];
					l = glm::distance(up->position, current->position);
					linkShape.push_back(Link(up, current, k, z, l)); //up link
				}

				//shearing links

				if (j + 1 < m_subdivision && i + 1 < m_subdivision)
				{
					Point* rightUp = &pointContainer[idx2DToIdx1D(i + 1, j + 1, m_subdivision)];
					l = glm::distance(rightUp->position, current->position);
					linkShearing.push_back(Link(current, rightUp, k, z, l)); //right up link
				}
				if (j - 1 > 0 && i + 1 < m_subdivision)
				{
					Point* rightDown = &pointContainer[idx2DToIdx1D(i + 1, j - 1, m_subdivision)];
					l = glm::distance(rightDown->position, current->position);
					linkShearing.push_back(Link(current, rightDown, k, z, l)); //right down link
				}

				//blending links : 

				if (i + 2 < m_subdivision)
				{
					Point* right2 = &pointContainer[idx2DToIdx1D(i + 2, j, m_subdivision)];
					l = glm::distance(right2->position, current->position);
					linkBlending.push_back(Link(current, right2, k, z, l)); //right link
				}

				if (j + 2 < m_subdivision)
				{
					Point* up2 = &pointContainer[idx2DToIdx1D(i, j + 2, m_subdivision)];
					l = glm::distance(up2->position, current->position);
					linkBlending.push_back(Link(current, up2, k, z, l)); //up link
				}
			}
		}

		//set masses : 
		for (int i = 0; i < m_subdivision; i++)
		{
			pointContainer[i].masse = 0.f;
		}

		for (int j = 1; j < m_subdivision; j++)
		{
			for (int i = 0; i < m_subdivision; i++)
			{
				pointContainer[idx2DToIdx1D(i, j, m_subdivision)].masse = 0.00008f / (float)(m_subdivision * m_subdivision);
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
		
		glm::vec3 min = pointContainer[0].position;
		glm::vec3 max = pointContainer[0].position;

		int verticePerFace = m_subdivision * m_subdivision;

		for (int i = 0, j = 0; i < pointContainer.size(); i++, j+=3)
		{
			m_mesh.vertices[j] = pointContainer[i].position.x + m_mesh.normals[j]*0.1f;
			m_mesh.vertices[j+1] = pointContainer[i].position.y + m_mesh.normals[j + 1] * 0.1f;
			m_mesh.vertices[j+2] = pointContainer[i].position.z + m_mesh.normals[j + 2] * 0.1f;

			m_mesh.vertices[j + verticePerFace*3] = pointContainer[i].position.x + m_mesh.normals[j + verticePerFace*3] * 0.1f;
			m_mesh.vertices[j + 1 + verticePerFace*3] = pointContainer[i].position.y + m_mesh.normals[j + 1 + verticePerFace*3] * 0.1f;
			m_mesh.vertices[j + 2 + verticePerFace*3] = pointContainer[i].position.z + m_mesh.normals[j + 2 + verticePerFace*3] * 0.1f;

			//calculate bounds : 
			if (pointContainer[i].position.x < min.x)
				min.x = pointContainer[i].position.x;
			if (pointContainer[i].position.y < min.y)
				min.y = pointContainer[i].position.y;
			if (pointContainer[i].position.z < min.z)
				min.z = pointContainer[i].position.z;

			if (pointContainer[i].position.x > max.x)
				max.x = pointContainer[i].position.x;
			if (pointContainer[i].position.y > max.y)
				max.y = pointContainer[i].position.y;
			if (pointContainer[i].position.z > max.z)
				max.z = pointContainer[i].position.z;
		}

		m_mesh.updateVBO(Mesh::VERTICES);
	
		updateNormals(); 
		
		//update bounds : 
		m_mesh.bottomLeft = min;
		m_mesh.topRight = max;
		auto collider = static_cast<BoxCollider*>(m_entity->getComponent(Component::ComponentType::COLLIDER));
		if (collider != nullptr)
		{
			//trick to remove scale and rotation from collider, because vertices are manually moved on scene and we want the collider to fit to the flag shape
			collider->scale = glm::vec3(1, 1, 1); 
			collider->rotation = glm::quat(0, 0, 0, 0);
			collider->coverMesh(m_mesh);
		}
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

	}

	void Flag::computePoints(float deltaTime, Point* point)
	{
		if (point->masse < 0.00000001f)
			return;

		//leapfrog
		point->vitesse += (deltaTime / point->masse)*point->force;
		point->position += deltaTime*point->vitesse;
		point->force = glm::vec3(0, 0, 0);
	}

	void Physic::Flag::render(const glm::mat4& projection, const glm::mat4& view)
	{
		glm::mat4 mvp = projection * view * modelMatrix;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

		m_material->use();
		m_material->setUniform_MVP(mvp);
		m_material->setUniform_normalMatrix(normalMatrix);

		m_mesh.draw();
	}

	void Flag::drawUI(Scene& scene)
	{
		if (ImGui::CollapsingHeader("flag"))
		{
			char tmpMaterialName[20];
			m_materialName.copy(tmpMaterialName, m_materialName.size());
			tmpMaterialName[m_materialName.size()] = '\0';

			if (ImGui::InputText("materialName", tmpMaterialName, 20))
			{
				m_materialName = tmpMaterialName;

				if (MaterialFactory::get().contains<Material3DObject>(m_materialName))
				{
					m_material = MaterialFactory::get().get<Material3DObject>(m_materialName);
				}
			}

			m_material->drawUI();
		}
	}

	void Flag::applyForce(const glm::vec3 & force)
	{
		for (int i = 0; i < pointContainer.size(); i++)
			pointContainer[i].force += force;
	}

	void Flag::applyGravity(const glm::vec3 & gravity)
	{
		for (int i = 0; i < pointContainer.size(); i++)
			pointContainer[i].force += (gravity * pointContainer[i].masse); // weight = m * g 
	}

	void Flag::eraseFromScene(Scene & scene)
	{
		scene.erase(this);
	}

	void Flag::addToScene(Scene & scene)
	{
		scene.add(this);
	}

	Component * Flag::clone(Entity * entity)
	{
		return nullptr;
	}

	Mesh & Flag::getMesh() 
	{
		return m_mesh;
	}

	void Flag::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
	{
		modelMatrix = glm::mat4_cast(rotation) * glm::scale(glm::mat4(1), scale);

		for (int i = 0; i < pointContainer.size(); i++)
		{
			pointContainer[i].position = glm::vec3(modelMatrix * glm::vec4(localPointPositions[i], 1.f));
		}

		modelMatrix = glm::translate(glm::mat4(1), translation);

		synchronizeVisual();
	}

	glm::vec3 Flag::getOrigin() const
	{
		return origin;
	}

}

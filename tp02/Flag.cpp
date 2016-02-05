#include "Flag.h"
#include "Scene.h"
#include "Entity.h"
#include "Factories.h"

namespace Physic {

	Flag::Flag(Material* material, int subdivision, float width, float height) : Component(FLAG), m_material(material), m_subdivision(subdivision), m_width(width), m_height(height)
	{

		//don't forget to change the origin to have the right pivot rotation
		origin = glm::vec3(-0.5f, -0.5f, 0.f);
		m_mesh.origin = glm::vec3(-0.5f, -0.5f, 0.f);

		float paddingX = m_width / (float)(m_subdivision - 1);
		float paddingY = m_height / (float)(m_subdivision - 1);

		int lineCount = (m_subdivision - 1);
		int rowCount = (m_subdivision - 1);
		m_mesh.triangleCount = (m_subdivision - 1) * (m_subdivision - 1) * 2 + 1;

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

		for (int j = 0; j < m_subdivision; j++)
		{
			for (int i = 0; i < m_subdivision; i++)
			{
				//physic elements : 
				localPointPositions.push_back(glm::vec3(i*paddingX, j*paddingY, 0.f));
				pointContainer.push_back( Point(glm::vec3(i*paddingX, j*paddingY, 0.f), glm::vec3(0,0,0), 0.f));

				//visual elements : 
				m_mesh.vertices.push_back(i*paddingX);
				m_mesh.vertices.push_back(j*paddingY);
				m_mesh.vertices.push_back(0);

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
		for (int j = 0; j < m_subdivision; j++)
		{
			for (int i = 0; i < m_subdivision; i++)
			{
				Point* current = &pointContainer[idx2DToIdx1D(i, j, m_subdivision)];

				//shape links

				if (i + 1 < m_subdivision)
				{
					Point* right = &pointContainer[idx2DToIdx1D(i + 1, j, m_subdivision)];
					linkShape.push_back(Link(current, right)); //right link
				}

				if (j + 1 < m_subdivision)
				{
					Point* down = &pointContainer[idx2DToIdx1D(i, j + 1, m_subdivision)];
					linkShape.push_back(Link(down, current)); //down link
				}

				//shearing links

				if (j + 1 < m_subdivision && i + 1 < m_subdivision)
				{
					Point* rightUp = &pointContainer[idx2DToIdx1D(i + 1, j + 1, m_subdivision)];
					linkShearing.push_back(Link(current, rightUp)); //right up link
				}
				if (j - 1 > 0 && i + 1 < m_subdivision)
				{
					Point* rightDown = &pointContainer[idx2DToIdx1D(i + 1, j - 1, m_subdivision)];
					linkShearing.push_back(Link(current, rightDown)); //right down link
				}

				//blending links : 

				if (i + 2 < m_subdivision)
				{
					Point* right2 = &pointContainer[idx2DToIdx1D(i + 2, j, m_subdivision)];
					linkBlending.push_back(Link(current, right2)); //right link
				}

				if (j + 2 < m_subdivision)
				{
					Point* up2 = &pointContainer[idx2DToIdx1D(i, j + 2, m_subdivision)];
					linkBlending.push_back(Link(current, up2)); //up link
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
				pointContainer[idx2DToIdx1D(i, j, m_subdivision)].masse = 0.5f;
			}
		}

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
		m_mesh.vertices.clear();

		for (int i = 0; i < pointContainer.size(); i++)
		{
			m_mesh.vertices.push_back(pointContainer[i].position.x);
			m_mesh.vertices.push_back(pointContainer[i].position.y);
			m_mesh.vertices.push_back(pointContainer[i].position.z);
		}

		m_mesh.initGl();
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

		m_material->setUniform_MVP(mvp);
		m_material->setUniform_normalMatrix(normalMatrix);
		m_material->use();

		m_mesh.draw();
	}

	void Flag::drawUI()
	{
		if (ImGui::CollapsingHeader("flag"))
		{
			char tmpMaterialName[20];
			m_materialName.copy(tmpMaterialName, m_materialName.size());
			tmpMaterialName[m_materialName.size()] = '\0';

			if (ImGui::InputText("materialName", tmpMaterialName, 20))
			{
				m_materialName = tmpMaterialName;

				if (MaterialFactory::get().contains(m_materialName))
				{
					m_material = MaterialFactory::get().get(m_materialName);
				}
			}
		}
	}

	void Flag::applyForce(const glm::vec3 & force)
	{
		for (int i = 0; i < pointContainer.size(); i++)
			pointContainer[i].force += force;
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
		modelMatrix = glm::translate(glm::mat4(1), translation) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1), scale);

		for (int i = 0; i < pointContainer.size(); i++)
		{
			pointContainer[i].position = glm::vec3(modelMatrix * glm::vec4(localPointPositions[i], 1.f));
		}

		synchronizeVisual();
	}

	glm::vec3 Flag::getOrigin() const
	{
		return origin;
	}

}

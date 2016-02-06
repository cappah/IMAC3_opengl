#pragma once

#include <vector>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "Point.h"
#include "Link.h"

#include "Utils.h"
#include "Mesh.h"
#include "Materials.h"
#include "Component.h"

namespace Physic {

	class Flag : public Component
	{
		glm::vec3 origin;
		glm::vec3 translation;
		glm::vec3 scale;
		glm::quat rotation;
		glm::mat4 modelMatrix;

		std::vector<glm::vec3> localPointPositions;
		std::vector<Point> pointContainer;
		std::vector<Link> linkShape; //maillages structurel (d4)
		std::vector<Link> linkShearing; //maillage diagonal (d8\d4)
		std::vector<Link> linkBlending; //maillage pont (tout les deux points)

		Mesh m_mesh;

		std::string m_materialName;
		Material *m_material;

		float m_width;
		float m_height;
		int m_subdivision;

	public:
		Flag(Material* material, int subdivision = 10, float width = 10.f, float height = 10.f);
		~Flag();

		void update(float deltaTime);
		void synchronizeVisual(); // update the mesh to follow the physic points

		void computeLinks(float deltaTime, Link* link);
		void computePoints(float deltaTime, Point* point);

		glm::vec3 getGravity() const;
		void setGravity(const glm::vec3& _gravity);
	
		void render(const glm::mat4& projection, const glm::mat4& view);

		void drawUI();

		void applyForce(const glm::vec3& force);
		//compute weight of each point, based on gravity and point mass, and add the weight force 
		void applyGravity(const glm::vec3& gravity);

		// Inherited via Component
		virtual void eraseFromScene(Scene & scene) override;
		virtual void addToScene(Scene & scene) override;
		virtual Component * clone(Entity * entity) override;

		Mesh& getMesh();

		virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1, 1, 1), const glm::quat& rotation = glm::quat()) override;

		glm::vec3 getOrigin() const;

	};

}

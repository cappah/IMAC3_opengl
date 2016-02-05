#pragma once

#include <vector>
#include <iostream>

#include "../glm/glm.hpp"
#include "../glm/common.hpp"

#include "Point.h"
#include "Link.h"

#include "Utils.h"
#include "Mesh.h"
#include "Materials.h"
#include "Component.h"

namespace Physic {

	class Flag : public Component
	{
		std::vector<Point> pointContainer;
		std::vector<Link> linkShape; //maillages structurel (d4)
		std::vector<Link> linkShearing; //maillage diagonal (d8\d4)
		std::vector<Link> linkBlending; //maillage pont (tout les deux points)

		Mesh m_mesh;

		std::string m_materialName;
		Material *m_material;

		glm::vec3 gravity;

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

		// Inherited via Component
		virtual void eraseFromScene(Scene & scene) override;
		virtual void addToScene(Scene & scene) override;
		virtual Component * clone(Entity * entity) override;
	};

}

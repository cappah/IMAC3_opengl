#pragma once

#include <vector>
#include "glm/common.hpp"

namespace Math {

	class BSpline
	{
	private:
		std::vector<glm::vec3> m_points;

	public:
		BSpline();
		~BSpline();

		void push_back(glm::vec3 p);
		void insert(int idx, glm::vec3 p);
		void pop_back();
		void remove(int idx);

		void clear();
		void append(const std::vector<glm::vec3>& points);

		int size();
	
		//return the position of the idx control point
		glm::vec3 getPoint(int idx);

		//return the position in the curve, betwwen the control points idx, idx + 1 and idx + 2, at position t (0 <= t < 1).
		glm::vec3 get(int idx, float t);

	};

	//return the point in the spline, between the control points P0, P1 and P2, at position t
	glm::vec3 getBSplinePoint(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, float t);

}


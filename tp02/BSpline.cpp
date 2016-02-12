#include "BSpline.h"


namespace Math
{

	BSpline::BSpline()
	{
	}


	BSpline::~BSpline()
	{
	}

	void BSpline::push_back(glm::vec3 p)
	{
		m_points.push_back(p);
	}

	void BSpline::insert(int idx, glm::vec3 p)
	{
		m_points.insert(m_points.begin() + idx, p);
	}

	void BSpline::pop_back()
	{
		m_points.pop_back();
	}

	void BSpline::remove(int idx)
	{
		m_points.erase(m_points.begin() + idx);
	}

	void BSpline::clear()
	{
		m_points.clear();
	}

	void BSpline::append(const std::vector<glm::vec3>& points)
	{
		for (auto& p : points)
			m_points.push_back(p);
	}

	int BSpline::size()
	{
		return m_points.size();
	}

	glm::vec3 BSpline::getPoint(int idx)
	{
		assert(idx >= 0 && idx < m_points.size());

		return m_points[idx];
	}

	glm::vec3 BSpline::get(int idx, float t)
	{
		assert(idx >= 0 && idx + 2 < m_points.size());

		glm::vec3 P0 = m_points[idx];
		glm::vec3 P1 = m_points[idx+1];
		glm::vec3 P2 = m_points[idx+2];

		return (P0 + P1) + t*(-2.f * P0 + 2.f * P1) + t*t*(P0 - 2.f * P1 + P2);
	}

}

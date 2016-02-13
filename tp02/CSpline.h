#pragma once

#include <vector>
#include "glm/common.hpp"

namespace Math {

	template<typename T>
	class CSpline
	{
	private:
		std::vector<T> m_points;

	public:
		CSpline();
		~CSpline();

		void push_back(T p);
		void insert(int idx, T p);
		void pop_back();
		void remove(int idx);

		void clear();
		void append(const std::vector<T>& points);

		int size();

		//return the position of the idx control point
		T getPoint(int idx) const;

		//return the position in the curve, betwwen the control points idx, idx + 1 and idx + 2, at position t (0 <= t < 1).
		T get(int idx, float t) const;

		//return the position in the curve, betwwen begin and end, at position t (0 <= t < 1).
		T get(float t) const;

	};

	//return the point in the spline, between the control points P0, P1 and P2, at position t
	template<typename T>
	T getCSplinePoint(const T& P0, const T& P1, const T& P2, const T& P3, float t);

}


namespace Math
{
	template<typename T>
	CSpline<T>::CSpline()
	{
	}

	template<typename T>
	CSpline<T>::~CSpline()
	{
	}

	template<typename T>
	void CSpline<T>::push_back(T p)
	{
		m_points.push_back(p);
	}

	template<typename T>
	void CSpline<T>::insert(int idx, T p)
	{
		m_points.insert(m_points.begin() + idx, p);
	}

	template<typename T>
	void CSpline<T>::pop_back()
	{
		m_points.pop_back();
	}

	template<typename T>
	void CSpline<T>::remove(int idx)
	{
		m_points.erase(m_points.begin() + idx);
	}

	template<typename T>
	void CSpline<T>::clear()
	{
		m_points.clear();
	}

	template<typename T>
	void CSpline<T>::append(const std::vector<T>& points)
	{
		for (auto& p : points)
			m_points.push_back(p);
	}

	template<typename T>
	int CSpline<T>::size()
	{
		return m_points.size();
	}

	template<typename T>
	T CSpline<T>::getPoint(int idx) const
	{
		assert(idx >= 0 && idx < m_points.size());

		return m_points[idx];
	}

	template<typename T>
	T CSpline<T>::get(int idx, float t) const
	{
		T P0 = m_points[idx];
		T P1 = m_points[idx + 1];
		T P2 = m_points[idx + 2];
		T P3 = m_points[idx + 3];

		return P1 + (t*(-0.5f*P0 + 0.5f*P2)) + (t*t*(P0 - 2.5f*P1 + 2*P2 -0.5f*P3)) + (t*t*t*(-0.5f * P0 + 1.5f * P1 - 1.5f * P2 + 0.5f*P3));
	}

	template<typename T>
	T CSpline<T>::get(float t) const
	{
		int idx = (int)(t * m_points.size() - 0.999f);

		T P0 = m_points[idx];
		T P1 = m_points[idx + 1];
		T P2 = m_points[idx + 2];
		T P3 = m_points[idx + 3];

		return P1 + (t*(-0.5f*P0 + 0.5f*P2)) + (t*t*(P0 - 2.5f*P1 + 2 * P2 - 0.5f*P3)) + (t*t*t*(-0.5f * P0 + 1.5f * P1 - 1.5f * P2 + 0.5f*P3));

	}

	template<typename T>
	T getCSplinePoint(const T& P0, const T& P1, const T& P2, const T& P3, float t)
	{
		return P1 + (t*(-0.5f * P0 + 0.5f * P2)) + (t*t*(P0 - 2.5f * P1 + 2.f * P2 - 0.5f * P3)) + (t*t*t*(-0.5f * P0 + 1.5f * P1 - 1.5f * P2 + 0.5f * P3));
	}

}


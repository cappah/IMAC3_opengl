#pragma once

#include <iostream>

#include "../glm/common.hpp"

#include "Point.h"

namespace Physic {

	struct Link
	{
		Point* M1;
		Point* M2;
		float k;
		float z;
		float l;
		float s1;
		float s2;
		float s3;

		Link();
		Link(Point* _M1, Point* _M2);
		Link(Point* _M1, Point* _M2, float _k, float _z, float _l, float _s1, float _s2, float _s3);
		~Link();

		void setConnection(Point* _M1, Point* _M2);
		void setParameters(float _k, float _z, float _l, float _s1, float _s2, float _s3);

	};

}

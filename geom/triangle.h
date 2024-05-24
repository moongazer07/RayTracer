#pragma once

#include "vector.h"
#include <initializer_list>
#include <cassert>

class Triangle {
public:
	Triangle(std::initializer_list<Vector> list) {
		assert(list.size() == 3);
		std::copy(list.begin(), list.end(), vertices_.begin());
	}

	Triangle(const Vector& a, const Vector& b, const Vector& c) : vertices_{a, b, c} 
	{}

	double Area() const {
		return Length(CrossProduct(vertices_[1] - vertices_[0], vertices_[2] - vertices_[0])) / 2.; 
	}

	const Vector& GetVertex(size_t ind) const {
		return vertices_[ind];
	}

private:
    std::array<Vector, 3> vertices_;
};
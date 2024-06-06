#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <cassert>

static double RandomDouble();

static double RandomDouble(double min, double max);

class Vector {
public:
    Vector() : Vector(0, 0, 0) {}

    Vector(double x, double y, double z) : Vector({x, y, z}) {}

    Vector(std::initializer_list<double> list) {
        assert (list.size() == 3);
        std::copy(list.begin(), list.end(), data_.begin());
    }

    Vector(double x) : Vector(x, x, x) {}

    Vector(std::array<double, 3> data) : data_(data) {}

    double& operator[](size_t ind) {
        return data_[ind];
    }

    double operator[](size_t ind) const {
        return data_[ind];
    }

    Vector& operator+=(const Vector& rhs) {
        data_[0] += rhs[0];
        data_[1] += rhs[1];
        data_[2] += rhs[2];
        return *this;
    }

    Vector operator*= (const Vector& rhs) {
        data_[0] *= rhs[0];
        data_[1] *= rhs[1];
        data_[2] *= rhs[2];
        return *this;
    }

    Vector& operator-=(const Vector& rhs) {
        data_[0] -= rhs[0];
        data_[1] -= rhs[1];
        data_[2] -= rhs[2];
        return *this;
    }

    Vector& operator /= (const Vector& rhs) {
        data_[0] /= rhs[0];
        data_[1] /= rhs[1];
        data_[2] /= rhs[2];
        return *this;
    }

    Vector& operator += (double r) {
        data_[0] += r;
        data_[1] += r;
        data_[2] += r;
        return *this;
    }

    Vector& operator -= (double r) {
        data_[0] -= r;
        data_[1] -= r;
        data_[2] -= r;
        return *this;
    }

    Vector& operator *= (double r) {
        data_[0] *= r;
        data_[1] *= r;
        data_[2] *= r;
        return *this;
    }

    Vector& operator /= (double r) {
        data_[0] /= r;
        data_[1] /= r;
        data_[2] /= r;
        return *this;
    }

    Vector operator- () const {
        return Vector(-data_[0], -data_[1], -data_[2]);
    }

    bool operator == (const Vector& rhs) const {
        return data_[0] == rhs[0] && data_[1] == rhs[1] && data_[2] == rhs[2];
    }

    bool operator != (const Vector& rhs) const {
        return !(*this == rhs);
    }

    friend double Length(const Vector& vec);

    friend double LengthSquared(const Vector& vec);


    void Normalize() {
        *this /= Length(*this);
    }

    static Vector RandomVector() {
        return Vector(RandomDouble(), RandomDouble(), RandomDouble());
    }

    static Vector RandomVector(double min, double max) {
        return Vector(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max));
    }

private:
    std::array<double, 3> data_;
};

double DotProduct(const Vector& lhs, const Vector& rhs);

Vector operator+ (const Vector& lhs, const Vector& rhs) {
    Vector tmp = lhs;
    tmp += rhs;
    return tmp;
}

Vector operator- (const Vector& lhs, const Vector& rhs) {
    Vector tmp = lhs;
    tmp -= rhs;
    return tmp;
}

Vector operator* (const Vector& lhs, const Vector& rhs) {
    Vector tmp = lhs;
    tmp *= rhs;
    return tmp;
}

Vector operator/ (const Vector& lhs, const Vector& rhs) {
    Vector tmp = lhs;
    tmp /= rhs;
    return tmp;
}

Vector operator+ (const Vector& lhs, double rhs) {
    Vector tmp = lhs;
    tmp += rhs;
    return tmp;
}

Vector operator+ (double lhs, const Vector& rhs) {
    return rhs + lhs;
}

Vector operator- (const Vector& lhs, double rhs) {
    Vector tmp = lhs;
    tmp -= rhs;
    return tmp;
}

Vector operator* (const Vector& lhs, double rhs) {
    Vector tmp = lhs;
    tmp *= rhs;
    return tmp;
}

Vector operator* (double lhs, const Vector& rhs) {
    return rhs * lhs;
}

Vector operator/ (const Vector& lhs, double rhs) {
    Vector tmp = lhs;
    tmp /= rhs;
    return tmp;
}

inline double Length(const Vector& vec) {
    return std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

inline double LengthSquared(const Vector& vec) {
    return vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];
}

Vector UnitVector(const Vector& vec) {
    Vector tmp = vec;
    tmp.Normalize();
    return tmp;
}

inline double RandomDouble() {
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    static std::mt19937 generator;
    return dist(generator);
}

inline double RandomDouble(double min, double max) {
    return min + (max - min) * RandomDouble();
}

inline Vector RandomInUnitSphere() {
    while (true) {
        auto p = Vector::RandomVector(-1, 1);
        if (LengthSquared(p) < 1) {
            return p;
        }
    }
}

inline Vector RandomInHemisphere(const Vector& normal) {
    Vector unit_sphere_vec = UnitVector(RandomInUnitSphere());
    if (DotProduct(normal, unit_sphere_vec) > 0) {
        return unit_sphere_vec;
    } else {
        return -unit_sphere_vec;
    }
}
 
inline double DotProduct(const Vector& lhs, const Vector& rhs) {
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
} 

inline Vector CrossProduct(const Vector& a, const Vector& b) {
    return {a[1] * b[2] - a[2] * b[1], 
            a[2] * b[0] - a[0] * b[2], 
            a[0] * b[1] - a[1] * b[0]};
}

void PrintVec(const Vector& vec) {
    std::cout << "(" << vec[0] << " " << vec[1] << " " << vec[2] << ")\n";
}

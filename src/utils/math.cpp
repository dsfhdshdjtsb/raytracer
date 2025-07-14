#include "math.h"
#include <cmath>

Tuple::Tuple(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Tuple::Tuple() : x(0), y(0), z(0), w(0) {}

bool Tuple::isPoint() const {
    return w == 1.0f;
}

bool Tuple::isVector() const {
    return w == 0.0f;
}


float Tuple::magnitude() const {
    return sqrt((x * x) + (y * y) + (z*z) + (w*w));
}
Tuple Tuple::normalize() const {
    float mag = this->magnitude();
    return Tuple(x / mag, y / mag, z / mag, w / mag);
}
float Tuple::dot(const Tuple& other) const {
    return x * other.x + y * other.y + z * other.z + w * other.w;
}
Vector Vector::cross(const Vector& other) const {
    return Vector(y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x);
}

Vector::Vector(float x, float y, float z) : Tuple(x, y, z, 0) {}

Point::Point(float x, float y, float z) : Tuple(x, y, z, 1) {}

bool Tuple::operator==(const Tuple& other) const {
    return (std::abs(x - other.x) < EPSILON) && 
           (std::abs(y - other.y) < EPSILON) && 
           (std::abs(z - other.z) < EPSILON) && 
           (std::abs(w - other.w) < EPSILON);
}

Tuple Tuple::operator+(const Tuple& other) const {
    return Tuple(x + other.x, y + other.y, z + other.z, w + other.w);
}

Tuple Tuple::operator-(const Tuple& other) const {
    return Tuple(x - other.x, y - other.y, z - other.z, w - other.w);
}

Tuple Tuple::operator-() const {
    return Tuple(-x, -y, -z, -w);
}


// Global operators for scalar multiplication from the left
Tuple operator*(float scalar, const Tuple& tuple) {
    return Tuple(tuple.x * scalar, tuple.y * scalar, tuple.z * scalar, tuple.w * scalar);
}

Tuple operator*(int scalar, const Tuple& tuple) {
    return Tuple(tuple.x * scalar, tuple.y * scalar, tuple.z * scalar, tuple.w * scalar);
}
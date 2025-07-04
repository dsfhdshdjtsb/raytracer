#include "math.h"

Tuple::Tuple(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Tuple::Tuple() : x(0), y(0), z(0), w(0) {}

bool Tuple::isPoint() const {
    return w == 1.0f;
}

bool Tuple::isVector() const {
    return w == 0.0f;
}

Vector::Vector(float x, float y, float z) : Tuple(x, y, z, 0) {}

Point::Point(float x, float y, float z) : Tuple(x, y, z, 1) {}
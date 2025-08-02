#include "shape.h"
#include "math.h"

bool Sphere::operator==(const Sphere& other) const {
    return r - other.r < EPSILON && center == other.center;
}


void Sphere::set_transform(Matrix t) {
    transform = transform * t;
}

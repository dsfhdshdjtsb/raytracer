#include <math.h>
#include "math.h"
#include "ray.h"
#include "shape.h"

Ray::Ray(Tuple origin, Tuple direction) : origin(origin), direction(direction) {}

Tuple Ray::position(float t) {
    return origin + direction * t;
}

Ray Ray::transform(const Matrix& m) const {
    return Ray(m * origin, m * direction);
}

std::vector<Intersection> Ray::intersect(Shape& s) {
    return s.intersect(*this);
}

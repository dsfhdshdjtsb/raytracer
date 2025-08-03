#include <math.h>
#include "math.h"
#include "ray.h"
#include "shape.h"

Ray::Ray(Tuple origin, Tuple direction) : origin(origin), direction(direction) {}

Tuple Ray::position(float t) const {
    return origin + direction * t;
}

Ray Ray::transform(const Matrix& m) const {
    return Ray(m * origin, m * direction);
}

std::vector<Intersection> Ray::intersect(const std::shared_ptr<Shape> shape) const {

    std::vector<float> ts = shape->intersect(*this);
    std::vector<Intersection> res;
    for(float t : ts) {
        res.push_back(Intersection(t, shape));
    }
    return res;
}


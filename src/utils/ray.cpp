#include <math.h>
#include "math.h"
#include "ray.h"
#include "shape.h"

Ray::Ray(Tuple origin, Tuple direction) : origin(origin), direction(direction) {}

Tuple Ray::position(double t) const {
    return origin + direction * t;
}

Ray Ray::transform(const Matrix& m) const {
    return Ray(m * origin, m * direction);
}

std::vector<Intersection> Ray::intersect(const std::shared_ptr<Shape> shape) const {

    std::vector<Intersection> res;
    if(auto group = std::dynamic_pointer_cast<Group>(shape)) {
        for(auto s : group->shapes)  {
            std::vector<double> ts = s->intersect(*this);
            for(double t : ts) {
                res.push_back(Intersection(t, s));
            }
        }
    } else {
        std::vector<double> ts = shape->intersect(*this);
        for(double t : ts) {
            res.push_back(Intersection(t, shape));
        }
    }
    return res;
}


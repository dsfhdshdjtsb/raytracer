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

//std::vector<Intersection> Ray::intersect(const std::shared_ptr<Shape> s) const {
    //return (*s).intersect(*this);
//}

std::vector<Intersection> Ray::intersect(const std::shared_ptr<Shape> shape) const {

    std::vector<float> ts = shape->intersect(*this);
    std::vector<Intersection> res;
    for(float t : ts) {
        res.push_back(Intersection(t, shape));
    }
    // Ray transformed = this->transform(shape->transform.inverse());
    // Tuple transformed_origin = transformed.origin;
    // Tuple transformed_direction = transformed.direction;

    // Tuple sphere_to_ray = transformed_origin - shape->center;

    // float a = transformed_direction.dot(transformed_direction);
    // float b = 2 * transformed_direction.dot(sphere_to_ray);
    // float c = sphere_to_ray.dot(sphere_to_ray) - shape->r * shape->r;

    // float discriminant = b * b - 4 * a * c;
    // if(discriminant < 0) {
    //     return {};
    // }

    // float t1 = ( -b - sqrt(discriminant)) / (2 * a);
    // float t2 = ( -b + sqrt(discriminant)) / (2 * a);
    return res;
}

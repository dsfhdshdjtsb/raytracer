#include <math.h>
#include "math.h"
#include "ray.h"

Tuple Ray::position(float t) {
    return origin + direction * t;
}

Ray Ray::transform(Matrix m) {
    return Ray(m * origin, m * direction);
}

std::vector<Intersection> Ray::intersect(Sphere s) {
    Ray transformed = this->transform(s.transform.inverse());
    Tuple transformed_origin = transformed.origin;
    Tuple transformed_direction = transformed.direction;

    Tuple sphere_to_ray = transformed_origin - s.center;

    float a = transformed_direction.dot(transformed_direction);
    float b = 2 * transformed_direction.dot(sphere_to_ray);
    float c = sphere_to_ray.dot(sphere_to_ray) - s.r * s.r;

    float discriminant = b * b - 4 * a * c;
    if(discriminant < 0) {
        return {};
    }

    float t1 = ( -b - sqrt(discriminant)) / (2 * a);
    float t2 = ( -b + sqrt(discriminant)) / (2 * a);

    return {Intersection(t1, s), Intersection(t2, s)};
}

Intersections::Intersections(std::initializer_list<Intersection> list) {
    for(Intersection i : list) {
        if(i.t >= 0) {
            pos.insert(i);
        } else {
            neg.insert(i);
        }
    }
}
Intersections::Intersections(std::vector<Intersection> list) {
    for(Intersection i : list) {
        if(i.t >= 0) {
            pos.insert(i);
        } else {
            neg.insert(i);
        }
    }
}

void Intersections::insert(Intersection is) {
    if(is.t >= 0) pos.insert(is);
    else neg.insert(is);
}

bool Intersection::operator==(const Intersection& other) const {
    if(t - other.t >= EPSILON) return false;
    
    return object == other.object;
}

Intersection Intersections::hit() {
    return *pos.begin();
}

bool IntersectionComparator::operator()(const Intersection& a, const Intersection& b) const {
    return a.t < b.t;
}
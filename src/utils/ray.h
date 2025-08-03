#ifndef RAY_INCLUDE
#define RAY_INCLUDE

#include "math.h"
#include <vector>
#include <memory>

struct Intersection;
struct Intersections;
struct Shape;

struct Ray {
    Tuple origin;
    Tuple direction;

    Ray(Tuple origin, Tuple direction);

    Tuple position(float t) const;
    Ray transform(const Matrix& m) const;
    std::vector<Intersection> intersect(const std::shared_ptr<Shape> c) const;
};


#endif
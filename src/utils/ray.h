#ifndef RAY_INCLUDE
#define RAY_INCLUDE

#include "math.h"
#include <vector>

struct Intersection;
struct Shape;

struct Ray {
    Tuple origin;
    Tuple direction;

    Ray(Tuple origin, Tuple direction);

    Tuple position(float t);
    Ray transform(const Matrix& m) const;
    std::vector<Intersection> intersect(Shape& c);
};

#endif
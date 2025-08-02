#ifndef RAY_INCLUDE
#define RAY_INCLUDE

#include "math.h"
#include "shape.h"
#include <set>

struct Intersection {
    Sphere object;
    float t;

    Intersection(float t, Sphere c) : t(t), object(c) {}

    bool operator==(const Intersection& other) const;
};

struct IntersectionComparator {
    bool operator()(const Intersection& a, const Intersection& b) const;
};

struct Intersections {
    std::set<Intersection, IntersectionComparator> pos;
    std::set<Intersection, IntersectionComparator> neg; 

    Intersections(std::initializer_list<Intersection> list);
    Intersections();
    Intersections(std::vector<Intersection> list);

    void insert(Intersection intersection);
    Intersection hit();
};

struct Ray {
    Tuple origin;
    Tuple direction;

    Ray(Tuple origin, Tuple direction) : origin(origin), direction(direction) {}

    Tuple position(float t);
    Ray transform(Matrix m);
    std::vector<Intersection> intersect(Sphere c);
};

#endif
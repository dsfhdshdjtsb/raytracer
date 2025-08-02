#ifndef SHAPE_INCLUDE
#define SHAPE_INCLUDE

#include "math.h"
#include <math.h>

struct Sphere {
    float r;
    Tuple center;
    Matrix transform;

    //Sphere(float r, Tuple c) : r(r), center(c) {}
    Sphere() : r(1), center(Point(0,0,0)), transform(IDENTITY_MATRIX) {}

    bool operator==(const Sphere& other) const;
    void set_transform(Matrix t);
};

#endif
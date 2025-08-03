#ifndef LIGHT_INCLUDE
#define LIGHT_INCLUDE

#include "math.h"

struct PointLight {
    Tuple intensity;
    Tuple position;

    PointLight(Tuple i, Tuple p);
};

#endif
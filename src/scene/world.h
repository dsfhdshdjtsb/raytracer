#ifndef WORLD_INCLUDE
#define WORLD_INCLUDE

#include "../utils/shape.h"
#include "../utils/light.h"

struct World {
    std::vector<std::shared_ptr<Shape>> objects;
    PointLight light;
    
    World();
    Intersections intersect_world(Ray& r) const;
};

#endif
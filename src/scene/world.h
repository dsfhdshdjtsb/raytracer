#ifndef WORLD_INCLUDE
#define WORLD_INCLUDE

#include <vector>
#include <memory>
#include "../utils/shape.h"
#include "../utils/math.h"
#include "../scene/camera.h"

struct Shape;
struct Canvas;
struct Computations;


struct World {
    std::vector<std::shared_ptr<Shape>> objects;
    PointLight light;
    Camera camera;
    
    World();
    Intersections intersect_world(const Ray& r) const;
    
    Tuple shade_hit(const Computations& comp) const;
    Tuple color_at(const Ray& r) const;

    Canvas render() const;
};

World DefaultWorld();
#endif